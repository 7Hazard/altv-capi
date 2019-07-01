#include "tool.hpp"

static auto templateClassMatcher = classTemplateSpecializationDecl(

    unless(isExpansionInSystemHeader())

    //,hasSpecializedTemplate(classTemplateDecl())
    //,hasAnyTemplateArgument(anything())

    ,unless(has(classTemplatePartialSpecializationDecl()))
    ,unless(has(classTemplateDecl()))
    ,unless(has(templateTypeParmType()))
    ,isDefinition()
).bind("template class");

/*
static Handler templateClassHandler(templateClassMatcher, [](const MatchFinder::MatchResult& result){
    auto record = result.Nodes.getNodeAs<ClassTemplateSpecializationDecl>("template class");

    // is invalid templated class
    //if(record->isTemplated() 
    //    && record->getTemplateSpecializationKind() 
    //        != TemplateSpecializationKind::TSK_ImplicitInstantiation
    //    )
    //{
    //    return;
    //}
    
    // Data
    auto recordnameorig = record->getTypeForDecl()
            ->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();

    std::string cstructname = ToCIdentifier(recordnameorig);

    //cstructname = std::regex_replace(cstructname, reg::sugarquals, "");
    //cstructname = std::regex_replace(cstructname, reg::spacestar, "Ptr");
    //cstructname = std::regex_replace(cstructname, reg::coloncolon_, "");
    //cstructname = std::regex_replace(cstructname, reg::notstartwithalt_, "");
    //cstructname = std::regex_replace(cstructname, reg::commaspace, "");
    //cstructname = std::regex_replace(cstructname, reg::templates, "");
    //cstructname = std::regex_replace(cstructname, reg::space, "");

    //cstructname = std::regex_replace(cstructname, reg::cstyle, "_$&");
    //cstructname = std::regex_replace(cstructname, reg::__more, "_");
    //cstructname = std::regex_replace(cstructname, reg::trailing_, "");
    //cstructname = ToLower(cstructname);

#ifdef _DEBUG
    auto loc = record->getLocation().printToString(*result.SourceManager);
    capicheader 
        << "// classTemplateSpecializationDecl" << std::endl
        << "// " << loc << std::endl
        << "// " << recordnameorig << std::endl;
#endif
    
    
    if(recordnameorig.find("std::") != std::string::npos)
    {
        logd("// Has relations with STD namespace\n");
        return;
    }
    else if(record->getDeclKind() == Decl::Kind::ClassTemplatePartialSpecialization)
    {
        #ifdef _DEBUG
        capicheader << "// Is ClassTemplatePartialSpecialization.\n" << std::endl;
        #endif
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

    auto dofields = [&](const RecordDecl* r){
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
            //else if(fieldtype->isPointerType())
            //{
            //    auto pointeetype = fieldtype->getPointeeType().getAsString();
            //    cfield = ToCType(field->getType().getCanonicalType().getAsString());
            //}
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
                //auto ctype = field->getType().getCanonicalType().getAsString();
                auto ctype = ToCType(
                    field->getType().getCanonicalType().getAsString()
                );

                //ctype = std::regex_replace(ctype, reg::sugarquals, "");
                //ctype = std::regex_replace(ctype, reg::coloncolon_, "");
                //ctype = std::regex_replace(ctype, reg::notstartwithalt_, "");
                //ctype = std::regex_replace(ctype, reg::commaspace, "");
                //ctype = std::regex_replace(ctype, reg::templates, "");

                //ctype = std::regex_replace(ctype, reg::cstyle, "_$&");
                //ctype = std::regex_replace(ctype, reg::__more, "_");
                //ctype = std::regex_replace(ctype, reg::trailing_, "");
                //ctype = ToLower(ctype);
                
                cfield = ("struct ")+ctype+(" ")+field->getCanonicalDecl()->getName().str();
            }

    #ifdef _DEBUG
            auto loc = field->getLocation().printToString(*result.SourceManager);
            body 
                << "    // " << loc << std::endl
                << "    // " << field->getType().getAsString() 
                    << " " << field->getName().str() << std::endl;
    #endif
            body 
                << "    " << cfield
                << ";\n";

            capijson["structs"][cstructname]["fields"].push_back({
                {"name", fieldname}
            });
        }

        return true;
    };

    // Inherited fields
    for(auto parent: record->bases())
    {
        if(parent.getType()->getAsCXXRecordDecl()->isInStdNamespace())
        {
            logd("// Parent is in STD namespace\n");
            return;
        }

        if(!dofields(parent.getType()->getAsRecordDecl()))
            return;
    }

    // Fields
    if(!dofields(record))
        return;

    capicheader << "typedef struct " << cstructname;
    if(body.str().empty()) capicheader << " " << cstructname << ";\n" << std::endl;
    else capicheader << " {\n" << body.str() << "} " << cstructname << ";\n" << std::endl;

    // Parent methods


    // Methods
    for(auto method : record->methods())
    {
        // Funcname, counter
        static std::unordered_map<std::string, size_t> funcnames;

        auto methodname = method->getDeclName().getAsString();
        std::string cfuncname;
        auto ret = method->getReturnType().getCanonicalType();
        std::string cret;

        #ifdef _DEBUG
        auto loc = method->getLocation().printToString(*result.SourceManager);
        logd("// Method" << std::endl
            << "// " << loc << std::endl
            << "// " << method->getQualifiedNameAsString() << std::endl
            << "// Returns " << ret.getAsString()
        );
        #endif

        if(method->getKind() == Decl::Kind::CXXConstructor)
        {
            logd("// Constructor\n");
            cfuncname = cstructname+"_Create";
            continue;
        }
        else if(method->getKind() == Decl::Kind::CXXDestructor)
        {
            logd("// Destructor\n");
            cfuncname = cstructname+"_Dispose";
            continue;
        }
        else if(method->isOverloadedOperator())
        {
            logd("// Overloaded operator\n");
            continue;
        }
        else if(ret.getAsString().find("std::") != std::string::npos)
        {
            logd("// Return type is in STD\n");
            continue;
        }
        //else if(ret.isPODType(*result.Context))
        //{
        //    logd("// Return type is not POD type\n");
        //    continue;
        //}
        else {
            cfuncname = cstructname + ("_") + methodname;
            if(funcnames.find(cfuncname) != funcnames.end())
                cfuncname+=std::to_string(funcnames[cfuncname]++);
            else {
                funcnames[cfuncname] = 1;
            }
        }

        if(ret->isFundamentalType() || ret->isBooleanType()
            || (ret->isPointerType() && ret->getPointeeType()->isFundamentalType())
            )
        {
            cret = ret.getAsString();
        }
        else if(ret->isEnumeralType())
        {
            cret = ToCType(ret.getAsString());
        }
        else {
            cret = ("struct ")+ToCType(ret.getAsString());
        }

        cfuncname = ToCIdentifier(cfuncname);
        std::string params;

        if(method->isStatic())
        {
            logd("// Static");
        }
        else {
            params += ("struct ")+cstructname+(" * _instance");
        }

        bool badparams = false;
        for(auto param : method->parameters())
        {
            auto paramtype = param->getType().getCanonicalType();
            auto paramname = param->getName().str();
            std::string cparam;
            logd("// Param " << paramtype.getAsString() << " " << paramname);

            if(paramtype.getAsString().find("std::") != std::string::npos)
            {
                logd("// Param type is in STD\n");
                badparams = true;
                continue;
            }
            else if(paramtype->isFunctionPointerType())
            {
                cparam = GetCFuncPtr(paramtype, paramname);
                if(cparam.empty())
                {
                    logd("// Bad funcptr\n");
                    badparams = true;
                    continue;
                }
            }
            else if(paramtype->isFundamentalType() || paramtype->isBooleanType()
                || (paramtype->isPointerType() && paramtype->getPointeeType()->isFundamentalType())
                )
            {
                cparam = paramtype.getAsString()+(" ")+paramname;
            }
            else if(paramtype->isEnumeralType())
            {
                cparam = ToCType(paramtype.getAsString())+(" ")+paramname;
            }
            else {
                cparam = ("struct ")+ToCType(paramtype.getAsString())+(" ")+paramname;
            }

            if(!params.empty())
                params+=", ";
            params += cparam;
        }

        if(badparams)
        {
            continue;
        }

        // Header
        capicheader <<
            "CAPI " << cret << " " << cfuncname << '(' << params << ");" << std::endl;

        // Source

        
        logd("");
    }

    // Add record to processed records
    //records.emplace(record, recordname);
    
    //record->dump(ast);
});
*/
