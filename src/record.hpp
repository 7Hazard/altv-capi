#include "tool.hpp"

#include <sstream>

static std::string GetCRecord(const MatchFinder::MatchResult& result, const CXXRecordDecl* record)
{
    std::stringstream ss;
    auto recordnameorig = record->getTypeForDecl()
            ->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();

    std::string cstructname = ToCIdentifier(recordnameorig);
    
    if(recordnameorig.find("std::") != std::string::npos)
    {
        logd("// Has relations with STD namespace\n");
        return "";
    }
    else if(record->getDeclKind() == Decl::Kind::ClassTemplatePartialSpecialization)
    {
        #ifdef _DEBUG
        ss << "// Is ClassTemplatePartialSpecialization.\n" << std::endl;
        #endif
        return "";
    }
    //else if(record->getDeclKind() == Decl::Kind::ClassTemplateSpecialization
    //    && (record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ExplicitInstantiationDefinition
    //    || record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ExplicitInstantiationDeclaration))
    //{
    //    #ifdef _DEBUG
    //    ss << "// Is Partial ClassTemplateSpecialization.\n" << std::endl;
    //    #endif
    //    return "";
    //}
    //else if(record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ImplicitInstantiation)
    //{
    //    #ifdef _DEBUG
    //    ss << "// Is ClassTemplate.\n" << std::endl;
    //    #endif
    //    return "";
    //}
    //else if(record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ImplicitInstantiation)
    //{
    //    logd("// Implicitly instanciated\n");
    //    return "";
    //}
    else if(record->getDeclKind() == Decl::Kind::ClassTemplate)
    {
        logd("// Is ClassTemplate.\n");
        return "";
    }
    else if(record->isTemplated() || record->isTemplateDecl())
    {
        logd("// isTemplated || isTemplateDecl.\n");
        return "";
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
            return "";
        }

        if(!dofields(parent.getType()->getAsRecordDecl()))
            return "";
    }

    // Fields
    if(!dofields(record))
        return "";

    ss << "typedef struct " << cstructname;
    if(body.str().empty()) ss << " " << cstructname << ";\n" << std::endl;
    else ss << " {\n" << body.str() << "} " << cstructname << ";\n" << std::endl;

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

                // AST DEBUG
                ast << "FROM FUNCTION " << cfuncname << "\n";
                astdump(paramtype);
            }
            else if(paramtype->isEnumeralType())
            {
                cparam = ToCType(paramtype.getAsString())+(" ")+paramname;
            }
            else {
                cparam = ("struct ")+ToCType(paramtype.getAsString())+(" ")+paramname;

                // AST DEBUG
                ast << "FROM FUNCTION " << cfuncname << "\n";
                astdump(paramtype);
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
        ss <<
            "CAPI " << cret << " " << cfuncname << '(' << params << ");" << std::endl;

        logd("");
    }

    // Add record to processed records
    //records.emplace(record, recordname);
    
    //record->dump(ast);

    return ss.str();
}
