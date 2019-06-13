#include "tool.hpp"

static auto recordMatcher = cxxRecordDecl(

    //fieldDecl().bind("record field")
    
    unless(isExpansionInSystemHeader())
    //,unless(isDerivedFrom(
    //    recordDecl(
    //        isInStdNamespace()
    //    )
    //))

    //,unless(classTemplateDecl())
    //,unless(isTemplateInstantiation())
    //,unless(isExplicitTemplateSpecialization())
    //,unless(isInstantiated())
    //,unless(templateSpecializationType())
    //,unless(isInTemplateInstantiation())
    //,unless(hasAnyTemplateArgument(anything()))
    //,unless(hasSpecializedTemplate(classTemplateDecl()))
    ,isDefinition()
).bind("record");


//static std::string ToCType(QualType& type, const clang::DeclContext *context)
//{
//    auto tname = type.getAsString();
//    if(type->isRecordType())
//    {
//        auto record = type->getAsCXXRecordDecl();
//        record = record->getTemplateInstantiationPattern();
//        if(!record)
//        {
//            return ToCType(type.getAsString());
//        }
//
//        auto rname = record->getQualifiedNameAsString();
//        auto tkind = record->getTemplateSpecializationKind();
//        ast << "ToCType" << '\n';
//        ast << ToCType(type.getAsString()) << '\n';
//        astdump(record);
//        return ToCType(type.getAsString());
//    }
//
//    return ToCType(type.getAsString());
//}

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
        #ifdef _DEBUG
        capiheader << "// Is ClassTemplatePartialSpecialization.\n" << std::endl;
        #endif
        return;
    }
    //else if(record->getDeclKind() == Decl::Kind::ClassTemplateSpecialization
    //    && (record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ExplicitInstantiationDefinition
    //    || record->getTemplateSpecializationKind() != TemplateSpecializationKind::TSK_ExplicitInstantiationDeclaration))
    //{
    //    #ifdef _DEBUG
    //    capiheader << "// Is Partial ClassTemplateSpecialization.\n" << std::endl;
    //    #endif
    //    return;
    //}
    //else if(record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ImplicitInstantiation)
    //{
    //    #ifdef _DEBUG
    //    capiheader << "// Is ClassTemplate.\n" << std::endl;
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

    if(typedefs.find(cstructname) != typedefs.end())
    {
        logd("// typedef already declared");
        return;
    }

    capiheader << "typedef struct " << cstructname;
    if(body.str().empty()) capiheader << " " << cstructname << ";\n" << std::endl;
    else capiheader << " {\n" << body.str() << "} " << cstructname << ";\n" << std::endl;

    auto domethods = [&](const CXXRecordDecl* r)
    {
        auto rname = r->getTypeForDecl()
            ->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();
        rname = std::regex_replace(rname, reg::classstructenum, "");

        for(auto method : r->methods())
        {
            // Funcname, counter
            static std::unordered_map<std::string, size_t> funcnames;

            auto methodname = method->getDeclName().getAsString();
            auto methodQualifiedName = method->getQualifiedNameAsString();
            std::string cfuncname;
            auto ret = method->getReturnType().getCanonicalType();
            std::string cret;
            std::stringstream cfuncsource;

            auto loc = method->getLocation().printToString(*result.SourceManager);
            logd("// Method" << std::endl
                << "// " << loc << std::endl
                << "// " << method->getQualifiedNameAsString() << std::endl
                << "// Returns " << ret.getAsString()
            );

            if(method->getAccess() == AccessSpecifier::AS_private
                || method->getAccess() == AccessSpecifier::AS_protected)
            {
                logd("// Not public\n");
                continue;
            }

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
            else if(method->isOverloadedOperator() || methodname.find("operator ") != std::string::npos)
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

            cfuncname = ToCIdentifier(cfuncname);
            std::string params;

            // Return type
            std::string paramReturnType;
            if(ret->isFundamentalType() || ret->isBooleanType()
                || (ret->isPointerType() && ret->getPointeeType()->isFundamentalType())
                )
            {
                cret = ret.getAsString();
                cfuncsource << "    return (" << cret << ")";
            }
            else if(ret->isEnumeralType())
            {
                cret = ToCType(ret.getAsString());
                cfuncsource << "    return (" << cret << ")";
            }
            else if(ret->isReferenceType())
            {
                cret = ("struct ")+ToCType(ret.getAsString());
                cfuncsource << "    return (" << cret << ")&";
            }
            else if(ret->isPointerType())
            {
                cret = ("struct ")+ToCType(ret.getAsString());
                cfuncsource << "    return (" << cret << ")";
            }
            else {
                paramReturnType = ToCType(ret.getAsString())+"*";
                cret = "void";
                cfuncsource << "    memcpy(_return, &";
            }

            // Static/Member
            if(method->isStatic())
            {
                logd("// Static");
                cfuncsource << rname << "::";
            }
            else {
                params += ("struct ")+cstructname+(" * _instance");
                cfuncsource << "((" << rname << " *)_instance)->";
            }
            cfuncsource << methodname << "(";

            // Params
            bool badparams = false;
            std::string cfuncSourceParams;
            for(auto param : method->parameters())
            {
                auto paramtype = param->getType().getCanonicalType();
                auto paramtypestr = paramtype.getAsString();
                auto paramname = param->getName().str();
                std::string cparam;
                logd("// Param " << paramtypestr << " " << paramname);

                if(!params.empty())
                    params+=", ";
                if(!cfuncSourceParams.empty())
                    cfuncSourceParams+=", ";

                if(paramtypestr.find("std::") != std::string::npos)
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
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else if(paramtype->isFundamentalType() || paramtype->isBooleanType()
                    || (paramtype->isPointerType() && paramtype->getPointeeType()->isFundamentalType())
                    )
                {
                    cparam = paramtypestr+(" ")+paramname;
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else if(paramtype->isEnumeralType())
                {
                    cparam = ToCType(paramtypestr)+(" ")+paramname;
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else if(paramtype->isPointerType() || paramtype->isReferenceType())
                {
                    cparam = ("struct ")+ToCType(paramtypestr)+(" ")+paramname;
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else {
                    // make it pointer type
                    cparam = ("struct ")+ToCType(paramtypestr)+("* ")+paramname;
                    cfuncSourceParams += ("*(")+paramtypestr+("*)")+paramname;

                    // AST DEBUG
                    //ast << "FROM FUNCTION PARAM " << cfuncname << "\n";
                    //astdump(param);

                    auto origtype = param->getOriginalType();
                    ast << "FROM FUNCTION PARAMTYPE " << cfuncname << "\n";
                    astdump(origtype);

                    ast << "FROM FUNCTION origtype desugared " << cfuncname << "\n";
                    auto t = origtype.getDesugaredType(param->getASTContext());
                    astdump(t);

                    //auto tspectype = dyn_cast_or_null<TypedefDecl>(
                    //    param->getOriginalType().getDesugaredType()
                    //);
                    //if(tspectype)
                    //{
                    //    ast << "FROM FUNCTION TemplateSpecializationType " << cfuncname << "\n";
                    //    astdump(tspectype);
                    //}

                    
                    
                    //auto tSpecType = dyn_cast_or_null<TemplateSpecializationType>(
                    //    param->getOriginalType()->getContainedDeducedType()
                    //);
                    //if(tSpecType)
                    //{
                    //    ast << "FROM FUNCTION TemplateSpecializationType " << cfuncname << "\n";
                    //    astdump(tSpecType);
                    //}

                    auto rec = param->getOriginalType()->getAsRecordDecl();
                    if(rec)
                    {
                        ast << "Record param FROM FUNCTION " << cfuncname << "\n";
                        astdump(paramtype->getAsRecordDecl());

                        auto spec = dyn_cast_or_null<ClassTemplateSpecializationDecl>(rec);
                        if(spec)
                        {
                            ast << "ClassTemplateSpecialization param FROM FUNCTION " << cfuncname 
                                << "\n";
                            astdump(spec);

                            auto def = spec->getDefinition();
                            if(def)
                            {
                                ast << "ClassTemplateSpecialization DEF param FROM FUNCTION " 
                                    << cfuncname << "\n";
                                astdump(def);
                            }
                        }
                    }
                }

                params += cparam;
            }
            cfuncsource << cfuncSourceParams << ")";

            if(badparams) // skip method
            {
                continue;
            }

            // Parametric return type
            if(!paramReturnType.empty())
            {
                if(!params.empty())
                    params+=", ";
                params += ("struct ")+paramReturnType+" _return";

                cfuncsource << ", sizeof(" << ret.getAsString() << "))";
            }

            cfuncsource << ";";

            // Header
            capiheader <<
                "CAPI " << cret << " " << cfuncname << '(' << params << ");"
                << std::endl << std::endl;

            // Source
            capisource <<
                "CAPI " << cret << " " << cfuncname << '(' << params << ")\n{\n" <<
                cfuncsource.str() <<
                "\n}\n\n";
        }
    };

    // Parent methods
    for(auto parent : record->bases())
    {
        if(parent.getType()->getAsCXXRecordDecl()->isInStdNamespace())
        {
            logd("// Parent is in STD namespace\n");
            return;
        }
        
        domethods(parent.getType()->getAsCXXRecordDecl());
    }

    // Methods
    domethods(record);



    // Add record to processed records
    //records.emplace(record, recordname);
    
    //record->dump(ast);
});
