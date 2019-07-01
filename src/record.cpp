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

    // free function
    auto cfreefuncsig = ("CAPI void ") + cstructname + ("_free(struct ") + cstructname + ("* ptr)");
    capiheader(cfreefuncsig << ";\n\n");
    capisource << cfreefuncsig << "\n{\n    delete ptr;\n}\n\n";

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
            std::stringstream cfunccomment;
            json cfuncjson;

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
            std::string crettype;
            std::string crettypekind;
            std::string paramReturnType;
            bool copyCtor = false;
            if(ret->isFundamentalType() || ret->isBooleanType()
                || (ret->isPointerType() && ret->getPointeeType()->isFundamentalType())
                )
            {
                if(ret->isPointerType() && ret->getPointeeType()->isFundamentalType()){
                    crettypekind = "pointer";
                }
                else {    
                    crettypekind = "fundemental";
                }
                crettype = ret.getAsString();
                cret = crettype;
                cfuncsource << "    return (" << cret << ")";
            }
            else if(ret->isEnumeralType())
            {
                crettypekind = "enum";
                crettype = ToCType(ret.getAsString());
                cret = ("enum ")+crettype;
                cfuncsource << "    return (" << cret << ")";
            }
            else if(ret->isReferenceType())
            {
                crettypekind = "pointer";
                crettype = ToCType(ret.getAsString());
                cret = ("struct ")+crettype;
                cfuncsource << "    return (" << cret << ")&";
            }
            else if(ret->isPointerType())
            {
                crettypekind = "pointer";
                crettype = ToCType(ret.getAsString());
                cret = ("struct ")+crettype;
                cfuncsource << "    return (" << cret << ")";
            }
            // copies
            else if(ret.isTriviallyCopyableType(method->getASTContext())){
                paramReturnType = ToCType(ret.getAsString())+"*";
                crettypekind = "fundemental";
                crettype = "void";
                cret = "void";
                cfuncsource << "    const auto& addr = ";
            }
            else {
                copyCtor = true;
                // cross your fingers and hope that the type has a copy constructor
                // allocate type via copy ctor
                crettypekind = "pointer";
                crettype = ToCType(ret.getAsString())+"*";
                cret = ("struct ")+crettype;
                cfuncsource << "    return (" << cret << ")(new " << ret.getAsString() << "(";

                // Has to have accompanying free function for the type
                // auto freefuncsig = ToCIdentifier(ret.getAsString())+("_free(")+cret+(" ptr)");
                auto freefuncsig = ToCIdentifier(ret.getAsString())+("_free()");
                cfunccomment << "// Return ptr must be manually freed with " << freefuncsig << "\n";
            }

            // return json
            cfuncjson["returns"] = {
                {"type", crettype},
                {"kind", crettypekind}
            };

            // Static/Member
            if(method->isStatic())
            {
                logd("// Static");
                cfuncsource << rname << "::";
            }
            else {
                std::string cparamtype = cstructname+"*";
                params += ("struct ")+cparamtype+(" _instance");
                cfuncsource << "((" << rname << " *)_instance)->";

                // json
                cfuncjson["params"].push_back({
                    {"name", "_instance"},
                    {"type", cparamtype},
                    {"kind", "pointer"}
                });
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
                std::string cparamtype;
                std::string cparamtypekind;
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
                    cparamtypekind = "function pointer";
                    cparam = GetCFuncPtr(paramtype, paramname);
                    cparamtype = std::regex_replace(cparam, std::regex((" ")+paramname), "");
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
                    cparamtypekind = "fundemental";
                    cparamtype = paramtypestr;
                    cparam = cparamtype+(" ")+paramname;
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else if(paramtype->isEnumeralType())
                {
                    cparamtypekind = "enum";
                    cparamtype = ToCType(paramtypestr);
                    cparam = ("enum ")+cparamtype+(" ")+paramname;
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else if(paramtype->isPointerType() || paramtype->isReferenceType())
                {
                    cparamtypekind = "pointer";
                    cparamtype = ToCType(paramtypestr);
                    cparam = ("struct ")+cparamtype+(" ")+paramname;
                    cfuncSourceParams += ("(")+paramtypestr+(")")+paramname;
                }
                else {
                    // make it pointer type
                    cparamtypekind = "pointer";
                    cparamtype = ToCType(paramtypestr)+("*");
                    cparam = ("struct ")+cparamtype+(" ")+paramname;
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

                cfuncjson["params"].push_back({
                    {"name", paramname},
                    {"type", cparamtype},
                    {"kind", cparamtypekind}
                });
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

                cfuncsource << ";\n    memcpy(_return, &addr, sizeof(" << ret.getAsString() << "))";
            }
            else if(copyCtor)
            {
                cfuncsource << "))";
            }

            cfuncsource << ";";

            // Header
            capiheader(cfunccomment.str()
                << "CAPI " << cret << " " << cfuncname << '(' << params << ");"
                << std::endl << std::endl);

            // Source
            capisource <<
                "CAPI " << cret << " " << cfuncname << '(' << params << ")\n{\n" <<
                cfuncsource.str() <<
                "\n}\n\n";

            // json
            cfuncjson["comment"] = cfunccomment.str();
            cfuncjson["struct"] = cstructname;
            capijson["functions"][cfuncname] = cfuncjson;
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



    // Add record to bookkept symbols
    capisymbols.emplace(cstructname);
    
    //record->dump(ast);
});
