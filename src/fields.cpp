#include "tool.hpp"

static auto recordMatcher = cxxRecordDecl(

    //fieldDecl().bind("record field")
    
    unless(isExpansionInSystemHeader())
    //,unless(isDerivedFrom(
    //    recordDecl(
    //        isInStdNamespace()
    //    )
    //))

    // ,isPublic()
    //,unless(classTemplateDecl())
    //,unless(isTemplateInstantiation())
    //,unless(isExplicitTemplateSpecialization())
    //,unless(isInstantiated())
    //,unless(templateSpecializationType())
    //,unless(isInTemplateInstantiation())
    //,unless(hasAnyTemplateArgument(anything()))
    //,unless(hasSpecializedTemplate(classTemplateDecl()))
    //,unless(isPrivate())
    ,isDefinition()
).bind("record");

static Handler recordHandler(recordMatcher, [](const MatchFinder::MatchResult& result){
    auto record = result.Nodes.getNodeAs<CXXRecordDecl>("record");
    
    // Data
    auto recordnameorig = record->getTypeForDecl()
            ->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();

    std::string cstructname = ToCIdentifier(recordnameorig);

    auto loc = record->getLocation().printToString(*result.SourceManager);
    logd(
        "// Record" << std::endl
        << "// " << loc << std::endl
        << "// " << recordnameorig
    );
    
    if(recordnameorig.find("std::") != std::string::npos)
    {
        logd("// Has relations with STD namespace\n");  
        return;
    }
    else if(record->getDeclKind() == Decl::Kind::ClassTemplatePartialSpecialization)
    {
        logd("// Is ClassTemplatePartialSpecialization.\n");
        return;
    }
    //else if(record->getDeclKind() == Decl::Kind::ClassTemplateSpecialization
    //    && (record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ExplicitInstantiationDefinition
    //    || record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ExplicitInstantiationDeclaration))
    //{
    //    #ifdef _DEBUG
    //    capicheader << "// Is Partial ClassTemplateSpecialization.\n" << std::endl;
    //    #endif
    //    return;
    //}
    //else if(record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ImplicitInstantiation)
    //{
    //    #ifdef _DEBUG
    //    capicheader << "// Is ClassTemplate.\n" << std::endl;
    //    #endif
    //    return;
    //}
    //else if(record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ImplicitInstantiation)
    //{
    //    logd("// Implicitly instanciated\n");
    //    return;
    //}
    else if(record->getDeclKind() == Decl::Kind::ClassTemplate)
    {
        logd("// Is ClassTemplate.\n");
        return;
    }
    else if(record->isTemplated() || record->isTemplateDecl())
    {
        logd("// isTemplated || isTemplateDecl.\n");
        return;
    }

    std::stringstream body;

    std::function<bool(const CXXRecordDecl*)> dofields = [&](const CXXRecordDecl* r){
        // Inherited fields
        for(auto parent: r->bases())
        {
            auto parentname = parent.getType().getAsString();
            logd("// derives from " << parentname);
            if(parent.getType()->getAsCXXRecordDecl()->isInStdNamespace())
            {
                logd("// Parent is in STD namespace\n");
                return false;
            }

            if(!dofields(parent.getType()->getAsCXXRecordDecl()))
                return false;
        }

        for(auto field : r->fields())
        {
            auto fieldtype = field->getType().getCanonicalType();
            auto fieldname = field->getName().str();
            std::string cfield;

            if(fieldtype.getAsString().find("std::") != std::string::npos)
            {
                logd("// Has fields in STD\n");
                return false;
            }
            if(fieldtype->isArrayType())
            {
                cfield = field->getType().getCanonicalType().getAsString();
                cfield.insert(cfield.rfind(" [")+1, fieldname);
            }
            else if(fieldtype->isFundamentalType() || fieldtype->isBooleanType()
                || (fieldtype->isPointerType() && fieldtype->getPointeeType()->isFundamentalType())
                )
            {
                cfield = fieldtype.getAsString()+(" ")+fieldname;
            }
            else if(fieldtype->isEnumeralType())
            {
                cfield = ("enum ")+ToCIdentifier(fieldtype.getAsString())+(" ")+fieldname;
            }
            else {
                auto ctype = ToCType(
                    field->getType().getCanonicalType().getAsString()
                );
                
                cfield = ("struct ")+ctype+(" ")+field->getCanonicalDecl()->getName().str();
            }
            
            auto loc = field->getLocation().printToString(*result.SourceManager);
            logd( 
                "    // " << loc << std::endl <<
                "    // " << field->getType().getAsString() <<
                " " << field->getName().str() << std::endl
            );

            body 
                << "    " << cfield
                << ";\n";

            capijson["structs"][cstructname]["fields"].push_back({
                {"name", fieldname}
            });
        }

        return true;
    };

    // Fields
    if(!dofields(record))
        return;

    if(capisymbols.find(cstructname) != capisymbols.end())
    {
        logd("// symbol already declared");
        return;
    }

    capixheader("typedef ", "");
    capiheader("struct " << cstructname);
    if(body.str().empty())
    {
        // capiheader(" " << cstructname << ";\n" << std::endl);
        capixheader(" " << cstructname << ";\n" << std::endl, " {};\n\n");
    }
    else
    {
        capixheader(
            " {\n" << body.str() << "} " << cstructname << ";\n" << std::endl,
            " {\n" << body.str() << "};\n" << std::endl
        );
    }

    // Add record to bookkept symbols
    capisymbols.emplace(cstructname);
    
    //record->dump(ast);
});
