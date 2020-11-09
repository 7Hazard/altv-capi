#include "tool.hpp"

static auto recordMatcher = cxxRecordDecl(

    //fieldDecl().bind("record field")
    
    unless(isExpansionInSystemHeader())
    //,unless(isDerivedFrom(
    //    recordDecl(
    //        isInStdNamespace()
    //    )
    //))
    
    // ,hasDescendant(
    //     cxxMethodDecl(
    //         isPublic()
    //     )
    // )
    // ,isPublic()
    // ,unless(isPrivate())
    // ,unless(isProtected())

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

// Funcname, counter
static std::unordered_map<std::string, size_t> funcnames;

static Handler recordHandler(recordMatcher, [](const MatchFinder::MatchResult& result){
    auto record = result.Nodes.getNodeAs<CXXRecordDecl>("record");

    static auto access = AccessSpecifier::AS_none;
    if(record->getAccess() != AccessSpecifier::AS_none)
    {
        access = record->getAccess();
    }
    
    // Data
    auto recordnameorig = record->getTypeForDecl()
            ->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();

    std::string cstructname = ToCIdentifier(recordnameorig);

    auto loc = record->getLocation().printToString(*result.SourceManager);
    logd(
        "// Record methods" << std::endl
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

    std::function<void(const CXXRecordDecl*, const CXXRecordDecl*)> domethods = [&](
        const CXXRecordDecl* r,
        const CXXRecordDecl* derived = nullptr
    )
    {
        // Parent methods
        for(auto parent : r->bases())
        {
            auto parentrecord = parent.getType()->getAsCXXRecordDecl();
            auto parentname = parentrecord->getIdentifier()->getName().str();

            static auto parentAccess = AccessSpecifier::AS_none;
            if(parentrecord->getAccess() != AccessSpecifier::AS_none)
            {
                parentAccess = parentrecord->getAccess();
            }

            if(parentAccess == AccessSpecifier::AS_private
                || parentAccess == AccessSpecifier::AS_protected)
            {
                logd("// derived record " << parentname << " is not publicly accessible" << std::endl);
                continue;
            }
            else {
                logd("// functions derived from " << parentname << std::endl);
            }

            if(parentrecord->isInStdNamespace())
            {
                logd("// Parent is in STD namespace\n");
                continue;
            }
            
            domethods(parentrecord, record);
        }

        auto recordname = r->getTypeForDecl()
            ->getLocallyUnqualifiedSingleStepDesugaredType()
            .getAsString();
        recordname = std::regex_replace(recordname, reg::classstructenum, "");
        auto crecordname = ToCIdentifier(recordname);
        // auto crecordname_altless = std::regex_replace(crecordname, reg::alt_, "");
        
        std::string derivedname, cderivedname, cderivedname_altless;
        if(derived)
        {
            derivedname = derived->getTypeForDecl()
                ->getLocallyUnqualifiedSingleStepDesugaredType()
                .getAsString();
            derivedname = std::regex_replace(derivedname, reg::classstructenum, "");
            cderivedname = ToCIdentifier(derivedname);
            // cderivedname_altless = std::regex_replace(cderivedname, reg::alt_, "");

            // Add conversion methods
            {
                // to derived
                auto convfuncname = crecordname + ("_to_") + cderivedname;
                auto convfuncsig = ("CAPI struct ") + cderivedname + ("* ") + 
                    convfuncname + ("(struct ") + crecordname + ("* from)");
                capiheader(convfuncsig << ";\n\n");
                capisource << convfuncsig << "\n{\n    return (" << cderivedname;
                if(derived->isPolymorphic())
                    capisource << "*)dynamic_cast<";
                else capisource << "*)static_cast<";
                capisource << derivedname << "*>((" 
                    << recordname << "*)from);\n}\n\n";

                if(funcnames.find(convfuncname) != funcnames.end())
                    convfuncname+=("_")+std::to_string(funcnames[convfuncname]++);
                else {
                    funcnames[convfuncname] = 1;
                }

                // json
                json cfuncjson;
                cfuncjson["comment"] = "";
                cfuncjson["struct"] = crecordname;
                cfuncjson["params"].push_back({
                    {"name", "from"},
                    {"type", {
                        {"name", crecordname+"*"},
                        {"kind", Typedata::POINTER},
                        {"kind_str", "pointer"}
                    }}
                });
                cfuncjson["returns"] = {
                    {"name", cderivedname+"*"},
                    {"kind", Typedata::POINTER},
                    {"kind_str", "pointer"}
                };
                capijson["functions"][convfuncname] = cfuncjson;
            }
            {
                // to parent
                auto convfuncname = cderivedname + ("_to_") + crecordname;
                auto convfuncsig = ("CAPI struct ") + crecordname + ("* ") + 
                    convfuncname + ("(struct ") + cderivedname + ("* from)");
                capiheader(convfuncsig << ";\n\n");
                capisource << convfuncsig << "\n{\n    return (" << crecordname 
                    << "*)static_cast<" << recordname << "*>((" 
                    << derivedname << "*)from);\n}\n\n";

                if(funcnames.find(convfuncname) != funcnames.end())
                    convfuncname+=("_")+std::to_string(funcnames[convfuncname]++);
                else {
                    funcnames[convfuncname] = 1;
                }

                // json
                json cfuncjson;
                cfuncjson["comment"] = "";
                cfuncjson["struct"] = cderivedname;
                cfuncjson["params"].push_back({
                    {"name", "from"},
                    {"type", {
                        {"name", cderivedname+"*"},
                        {"kind", Typedata::POINTER},
                        {"kind_str", "pointer"}
                    }}
                });
                cfuncjson["returns"] = {
                    {"name", crecordname+"*"},
                    {"kind", Typedata::POINTER},
                    {"kind_str", "pointer"}
                };
                capijson["functions"][convfuncname] = cfuncjson;
            }
        }

        for(auto method : r->methods())
        {
            auto methodname = method->getNameInfo().getAsString();
            auto methodQualifiedName = method->getQualifiedNameAsString();

            std::stringstream csharedfuncbody;
            std::stringstream csharedfunccomment;

            std::string cfuncname;
            std::stringstream cfuncbody;
            std::stringstream cfunccomment;
            json cfuncjson;

            bool hasHeapFunc = false;
            std::stringstream cheapfuncbody;
            std::stringstream cheapfunccomment;

            bool isTypecastOverload = methodname.find("operator ") != std::string::npos;

            auto loc = method->getLocation().printToString(*result.SourceManager);
            logd("// Method" << std::endl
                << "// " << loc << std::endl
                << "// " << method->getQualifiedNameAsString()
            );

            if(method->getAccess() == AccessSpecifier::AS_private
                || method->getAccess() == AccessSpecifier::AS_protected)
            {
                logd("// Not public\n");
                continue;
            }

            if(method->isDeleted())
            {
                logd("// Deleted");
                continue;
            }

            if(method->getKind() == Decl::Kind::CXXConstructor)
            {
                logd("// Constructor");
                if(r->isAbstract())
                {
                    logd("// class is abstract");
                    continue;
                }
                // else if (derived)
                //     logd("// constructor is from parent");
                cfuncname = cstructname+"_Create";
            }
            else if(method->getKind() == Decl::Kind::CXXDestructor)
            {
                logd("// Destructor\n");
                cfuncname = cstructname+"_Dispose";
                continue;
            }
            else if(isTypecastOverload)
            {
                logd("// Typecast overload");

                auto info = ToCIdentifier(
                    method->getReturnType()
                        .getCanonicalType()
                        .getDesugaredType(method->getASTContext())
                        .getUnqualifiedType()
                        .getAsString()
                );
                cfuncname = cstructname+"_To_"+info;
            }
            else if(method->isOverloadedOperator())
            {
                logd("// Overloaded operator");
                    
                if(methodname == "operator=")
                {
                    auto info = ToCIdentifier(
                        method->getParamDecl(0)->getType()
                            ->getLocallyUnqualifiedSingleStepDesugaredType()
                            .getAsString()
                    );
                    cfuncname = cstructname+"_Assign_"+info;
                }
                else if(methodname == "operator==")
                {
                    auto info = ToCIdentifier(
                        method->getParamDecl(0)->getType()
                            ->getLocallyUnqualifiedSingleStepDesugaredType()
                            .getAsString()
                    );
                    cfuncname = cstructname+"_Equals_"+info;
                }
                else if(methodname == "operator!=")
                {
                    auto info = ToCIdentifier(
                        method->getParamDecl(0)->getType()
                            ->getLocallyUnqualifiedSingleStepDesugaredType()
                            .getAsString()
                    );
                    cfuncname = cstructname+"_NotEquals_"+info;
                }
                else if(methodname == "operator[]")
                {
                    // auto info = ToCIdentifier(
                    //     method->getParamDecl(0)->getType()
                    //         ->getLocallyUnqualifiedSingleStepDesugaredType()
                    //         .getAsString()
                    // );
                    auto type = Typedata(
                        method->getParamDecl(0)->getType()->getLocallyUnqualifiedSingleStepDesugaredType(),
                        record->getASTContext()
                    );
                    cfuncname = cstructname+"_Access_"+ToCIdentifier(type.ctype);
                }
                else {    
                    continue;
                }
            }
            else {
                cfuncname = cstructname + ("_") + methodname;
            }

            // Clean C name, check for overloads
            cfuncname = ToCIdentifier(cfuncname);
            if(funcnames.find(cfuncname) != funcnames.end())
                cfuncname+=("_")+std::to_string(funcnames[cfuncname]++);
            else {
                funcnames[cfuncname] = 1;
            }

            std::string headerparams;

            auto rettype = method->getReturnType().getCanonicalType();
            auto rettypeUnqualified = method->getReturnType().getCanonicalType().getUnqualifiedType();
            logd("// Return type");

            auto retdata = Typedata(rettype, record->getASTContext());
            auto heapretdata = Typedata(rettype, record->getASTContext());
            if(!retdata.ok)
                continue;

            std::string stmtclose = ");";
            std::string heapstmtclose = ");";
            if(method->getKind() == Decl::Kind::CXXConstructor)
            {
                // TODO, maybe set pointee value in retdata?

                // Stack version
                retdata.kind = Typedata::FUNDAMENTAL;
                retdata.ctype = "void";
                retdata.forwardDecl = "";
                cfuncbody << "*((" << recordname << "*)_returnValue) = " << recordname << "(";

                // Heap version
                hasHeapFunc = true;
                heapretdata.kind = Typedata::POINTER;
                heapretdata.ctype = cstructname+"*";
                heapretdata.forwardDecl = "struct ";
                cheapfuncbody << "return (" << heapretdata.forwardDecl
                    << heapretdata.ctype << ")new " << recordname << "(";

                // Has to have accompanying free function for the type
                auto freefuncsig = cstructname+("_CAPI_Free()");
                cheapfunccomment << ("// Return ptr must be manually freed with ") + freefuncsig + "\n";
            }
            else if(methodname == "operator=")
            {
                retdata = Typedata(retdata, Typedata::FUNDAMENTAL);
                // cfuncbody <<  "    *((" << recordname << " *)_instance) = (";
            }
            else if(retdata.kind == Typedata::STRUCT)
            {
                // don't return a struct by value
                // if(rettype.isTriviallyCopyableType(method->getASTContext()))
                // {
                //     // copy the values
                //     // VOIDIFY
                //     retdata = Typedata(retdata, Typedata::FUNDEMENTAL);
                //     cfuncbody << "    const auto& addr = ";
                // }
                // else
                {
                    // cross your fingers and hope that the type has a copy constructor
                    // heap allocate type via copy ctor, requires manual freeing

                    heapretdata = Typedata(retdata, Typedata::POINTER);
                    
                    // Stack version
                    retdata.kind = Typedata::FUNDAMENTAL;
                    retdata.ctype = "void";
                    retdata.forwardDecl = "";
                    cfuncbody << "*((" << rettypeUnqualified.getAsString() << "*)_returnValue) = ";
                    
                    // Heap version
                    hasHeapFunc = true;
                    cheapfuncbody << "return (" << heapretdata.ctype << ")new " << rettype.getAsString() << "(";
                    heapstmtclose = "));";

                    // Has to have accompanying free function for the type
                    auto freefuncsig = ToCIdentifier(rettype.getAsString())+("_CAPI_Free()");
                    cheapfunccomment << "// Return ptr must be manually freed with " << freefuncsig << "\n";
                }
            }
            else if(retdata.kind == Typedata::REFERENCE)
            {
                cfuncbody << "return (" << retdata.ctype << ")&";
            }
            else {
                // just return the value
                
                if(retdata.ctype != "void")
                {
                    cfuncbody << "return ";
                    cfuncbody << "(" << retdata.ctype << ")";
                }
            }

            if(
                method->getKind() == Decl::Kind::CXXConstructor
                // || methodname == "operator="
            )
            {
                // do nothing
            }
            else if(method->isStatic())
            {
                logd("// Static method");
                if(derived)
                    csharedfuncbody << derivedname << "::" << methodname << "(";
                else 
                    csharedfuncbody << recordname << "::" << methodname << "(";
            }
            else {
                std::string cparamtype = cstructname+"*";
                headerparams += ("struct ")+cparamtype+(" _instance");

                if(derived)
                {
                    csharedfuncbody << "static_cast<" << recordname << " *>((" << derivedname 
                        << " *)_instance)->" << methodname << "(";
                }
                else
                {
                    csharedfuncbody << "((" << recordname 
                        << " *)_instance)->" << methodname << "(";
                }

                // json
                cfuncjson["params"].push_back({
                    {"name", "_instance"},
                    {"type", {
                        {"name", cparamtype},
                        {"kind", Typedata::POINTER},
                        {"kind_str", "pointer"}
                    }}
                });
            }

            // return json
            cfuncjson["returns"] = retdata.json_data();

            // Params
            bool badparams = false;
            std::string sourceparams;
            for(auto param : method->parameters())
            {
                auto paramname = param->getName().str();
                if(paramname.empty())
                    paramname = ("_p")+std::to_string(param->getFunctionScopeIndex());
                auto origparamtype = param->getType().getUnqualifiedType();
                auto paramtype = origparamtype.getCanonicalType()
                    .getUnqualifiedType();
                auto typedata = Typedata(paramtype, record->getASTContext());
                auto origparamtypestr = std::regex_replace(param->getType().getAsString(), reg::classstructenum, "");
                
                if(!typedata.ok)
                {
                    badparams = true;
                    continue;
                }

                if(!headerparams.empty())
                    headerparams+=", ";
                if(!sourceparams.empty())
                    sourceparams+=", ";

                if(typedata.kind == Typedata::STRUCT)
                {
                    // take a pointer to the struct and dereference it on use
                    typedata = Typedata(typedata, Typedata::POINTER);
                    headerparams += typedata.forwardDecl + typedata.ctype
                        + (" ") + paramname;
                    sourceparams += ("*(")+origparamtypestr+("*)")
                        +paramname;
                }
                else if(typedata.kind == Typedata::REFERENCE)
                {
                    // make it a pointer
                    typedata.kind = Typedata::POINTER;
                    // take a pointer to the value and dereference it on use
                    headerparams += typedata.forwardDecl + typedata.ctype
                        + (" ") + paramname;
                    sourceparams += ("*(")+std::regex_replace(origparamtypestr, reg::ref, "*")+(")")
                        +paramname;
                }
                else if(typedata.kind == Typedata::FUNCTION_POINTER)
                {
                    auto fn = typedata.forwardDecl + typedata.ctype;
                    fn = std::regex_replace(fn, reg::fnptrname, ("(*")+paramname+")");
                    
                    headerparams += fn;
                    sourceparams += ("(")+origparamtypestr+(")")
                        +paramname;
                }
                else {
                    headerparams += typedata.forwardDecl + typedata.ctype
                        + (" ") + paramname;
                    sourceparams += ("(")+origparamtypestr+(")")
                        +paramname;
                }

                cfuncjson["params"].push_back({
                    {"name", paramname},
                    {"type", typedata.json_data()}
                });
            }

            if(badparams)
                continue;

            // Parametric return type
            // if(retdata.kind == Typedata::STRUCT
            //     && rettype.isTriviallyCopyableType(method->getASTContext()))
            // {
            //     if(!params.empty())
            //         params+=", ";
            //     params += ("struct ")+paramReturnType+" _return";
            //     // TODO ADD JSON TO PARAMS
            //     cfuncsource << ";\n    memcpy(_return, &addr, sizeof(" << rettype.getAsString() << "))";
            // }

            csharedfuncbody << sourceparams;

            // If has heap function, then func is a stack version
            // Handle parameters accordingly
            auto cheapheaderparams = headerparams;
            json cheapfuncjson;
            if(hasHeapFunc)
            {
                cheapfuncjson = cfuncjson;
                cfuncjson["params"].push_back({
                    {"name", "returnValue"},
                    {"type", heapretdata.json_data()}
                });
                if(!headerparams.empty()) headerparams += ", ";
                headerparams += heapretdata.forwardDecl + heapretdata.ctype + " _returnValue";
            }

            cfuncbody << csharedfuncbody.str() << stmtclose;

            auto funcsig = ("CAPI ") 
                + retdata.forwardDecl
                + retdata.ctype
                + " " + cfuncname
                + '(' + headerparams + ")";

            // Header
            capiheader(cfunccomment.str()
                << funcsig << ";" 
                << std::endl << std::endl);

            // Source
            capisource
                << funcsig << "\n{\n    " << cfuncbody.str() << "\n}\n\n";

            // json
            cfuncjson["comment"] = cfunccomment.str();
            cfuncjson["struct"] = cstructname;
            capijson["functions"][cfuncname] = cfuncjson;

            if(hasHeapFunc)
            {
                cfuncname += "_CAPI_Heap";
                cheapfuncbody << csharedfuncbody.str() << heapstmtclose;
                auto heapfuncsig = ("CAPI ")
                    + heapretdata.forwardDecl
                    + heapretdata.ctype
                    + " " + cfuncname
                    + '(' + cheapheaderparams + ")";

                // Header
                capiheader(cheapfunccomment.str()
                    << heapfuncsig << ";" 
                    << std::endl << std::endl);

                // Source
                capisource
                    << heapfuncsig << "\n{\n    " << cheapfuncbody.str() << "\n}\n\n";

                // json
                cheapfuncjson["returns"] = heapretdata.json_data();
                cheapfuncjson["comment"] = cheapfunccomment.str();
                cheapfuncjson["struct"] = cstructname;
                capijson["functions"][cfuncname] = cheapfuncjson;

                funcnames[cfuncname] = 1;
            }
        }

        capiheader("\n");
    };

    // if(cstructname.find("String") != std::string::npos)
    // {
    //     logd("// GDGDF");
    // }
    // if(cstructname.find("Storage") != std::string::npos)
    // {
    //     logd("// STORAGE CLASS");
    // }

    // static std::unordered_set<std::string> privateRecords;
    // auto describedRecord = record->getTemplateInstantiationPattern();
    // std::string describedRecordName;
    // if(describedRecord)
    // {
    //     describedRecordName = describedRecord->getDeclName().getAsString();

    //     auto access = describedRecord->getAccess();
    //     if(describedRecord->getAccess() == AccessSpecifier::AS_private
    //         || describedRecord->getAccess() == AccessSpecifier::AS_protected)
    //     {
    //         logd("// OAGM");
    //     }

    //     // just, i can't deal with these yet
    // }
    
    // Methods
    // if(describedRecord
    //     && privateRecords.find(describedRecordName) != privateRecords.end())
    // {
    //     logd("// templated record");
    //     logd("// the record and its methods are not publicly accessible\n");
    // }
    if(recordnameorig.find("alt::MValue::Storage") != std::string::npos)
    {
        // can't get proper access info for template instanciated records
        // this is a horrible temporary workaround for current cpp-sdk
        // https://bugs.llvm.org/show_bug.cgi?id=42658

        logd("// BAD RECORD\n");
        // return;
    }
    else if(record->getAccess() == AccessSpecifier::AS_private
        || record->getAccess() == AccessSpecifier::AS_protected)
    {
        logd("// the record and its methods are not publicly accessible\n");

        // auto rn = record->getIdentifier()->getName().str();
        // privateRecords.insert(
        //     rn
        // );
    }
    else if(record->isLambda())
    {
        logd("// record is a lambda\n")
    }
    else
    // if(
    //     (record->isStruct() && currentAccess == AccessSpecifier::AS_none)
    //     || (record->isStruct() && currentAccess == AccessSpecifier::AS_public)
    //     || (record->isClass() && currentAccess == AccessSpecifier::AS_public)
    // )
    {
        // if(cstructname.find("Storage") != std::string::npos)
        // {
        //     logd("// STORAGE CLASS");
        // }

        logd("// functions from " << recordnameorig);

        // free function
        if(record->hasSimpleDestructor() || (!record->hasIrrelevantDestructor() && !record->needsOverloadResolutionForDestructor() && record->getDestructor()->getAccessUnsafe() == AccessSpecifier::AS_public))
        {
            // Destructor is accessible
            auto freefuncname = cstructname + ("_CAPI_Free");
            auto cfreefuncsig = ("CAPI void ") + freefuncname + ("(struct ") + cstructname + ("* ptr)");
            capiheader(cfreefuncsig << ";\n\n");
            capisource << cfreefuncsig << "\n{\n    delete (" 
                << recordnameorig << " *)ptr;\n}\n\n";

            if(funcnames.find(freefuncname) != funcnames.end())
                freefuncname+=("_")+std::to_string(funcnames[freefuncname]++);
            else {
                funcnames[freefuncname] = 1;
            }

            // json
            json cfuncjson;
            cfuncjson["comment"] = "";
            cfuncjson["struct"] = cstructname;
            cfuncjson["params"].push_back({
                {"name", "ptr"},
                {"type", {
                    {"name", cstructname+"*"},
                    {"kind", Typedata::POINTER},
                    {"kind_str", "pointer"}
                }}
            });
            cfuncjson["returns"] = {
                {"name", "void"},
                {"kind", Typedata::FUNDAMENTAL},
                {"kind_str", "fundamental"}
            };
            capijson["functions"][freefuncname] = cfuncjson;
        }
        else
        {
            logd("// has no accessible destructor for 'Free' function\n");
        }

        domethods(record, nullptr);
    }
});
