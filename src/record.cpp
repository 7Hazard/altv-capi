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
    // ,unless(isPrivate())
    ,isDefinition()
).bind("record");

struct Typedata
{
    enum Kind
    {
        FUNDEMENTAL,
        ENUMERAL,
        REFERENCE,
        POINTER,
        ARRAY,
        STRUCT
    } kind;

    bool ok = true;
    Typedata* pointee = nullptr;
    std::string ctype;
    std::string forwardDecl;

    Typedata(clang::QualType type, const clang::ASTContext& context)
    {
        type = type.getCanonicalType()
            .getUnqualifiedType()
            .getDesugaredType(context);

        auto cpptypestr = type.getAsString();
        logd("// cpptype " << cpptypestr);

        if(cpptypestr.find("std::") != std::string::npos)
        {
            logd("// type is in STD\n");
            ok = false;
            return;
        }
        else if(type->isFunctionPointerType())
        {
            // todo
            abort();
        }
        else if(type->isFundamentalType() || type->isBooleanType())
        {
            logd("// fundamental");
            kind = FUNDEMENTAL;
            ctype = cpptypestr;
        }
        else if(type->isEnumeralType())
        {
            logd("// enumaral type");
            kind = ENUMERAL;
            forwardDecl = "enum ";
            ctype = ToCType(cpptypestr);
        }
        else if(type->isArrayType())
        {
            logd("// array type");
            kind = ARRAY;
            pointee = new Typedata(
                type->getPointeeOrArrayElementType()
                    ->getCanonicalTypeUnqualified()
                    .operator clang::QualType(),
                context
            );
            ctype = pointee->ctype+"*";

            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl; 
        }
        else if(type->isReferenceType())
        {
            logd("// Reference type");
            kind = REFERENCE;
            pointee = new Typedata(type->getPointeeType(), context);
            ctype = pointee->ctype+"*";

            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl;
        }
        else if(type->isPointerType())
        {
            logd("// Pointer type");
            kind = POINTER;
            pointee = new Typedata(type->getPointeeType(), context);

            ctype = pointee->ctype+"*";

            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl;
        }
        else if(type->isStructureOrClassType())
        {
            kind = STRUCT;
            forwardDecl = "struct ";
            ctype = ToCType(cpptypestr);
        }
        else
        {
            logd("// Could not process type");
            abort();
        }
    }

    // Manual typing
    // FUNDEMENTAL kind will use _ctype param for fundemental type
    Typedata(Typedata& data, Typedata::Kind kind, std::string _ctype = "void")
        : kind(kind)
    {
        switch (kind)
        {
        case POINTER:
        {
            pointee = &data;
            ctype = pointee->ctype+"*";
            
            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl;
        } break;

        case FUNDEMENTAL:
        {
            ctype = _ctype;
            forwardDecl = "";
            pointee = nullptr;
        } break;
        
        default:
        {
            abort();
        } break;
        }
    }
};

// Funcname, counter
static std::unordered_map<std::string, size_t> funcnames;

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

            /* debug */
            auto ispublic = parentrecord->getAccess() == AccessSpecifier::AS_public;
            auto isnone = parentrecord->getAccess() == AccessSpecifier::AS_none;
            auto isvisible = parentrecord->getVisibility() == Visibility::DefaultVisibility;
            /* debug */

            if(
                parentrecord->getAccess() == AccessSpecifier::AS_public
                && parentrecord->getAccess() == AccessSpecifier::AS_none
            )
            {
                logd("// functions derived from " << parentname << std::endl);
            }
            else {
                logd("// derived record " << parentname << " is not publicly accessible" << std::endl);
                continue;
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
        
        std::string derivedname;
        if(derived)
        {
            derivedname = derived->getTypeForDecl()
                ->getLocallyUnqualifiedSingleStepDesugaredType()
                .getAsString();
            derivedname = std::regex_replace(derivedname, reg::classstructenum, "");
        }

        for(auto method : r->methods())
        {
            auto methodname = method->getDeclName().getAsString();
            auto methodQualifiedName = method->getQualifiedNameAsString();
            std::string cfuncname;
            std::stringstream cfuncbody;
            std::stringstream cfunccomment;
            json cfuncjson;

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
                    auto info = ToCIdentifier(
                        method->getParamDecl(0)->getType()
                            ->getLocallyUnqualifiedSingleStepDesugaredType()
                            .getAsString()
                    );
                    cfuncname = cstructname+"_Access_"+info;
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
            logd("// Return type");

            auto retdata = Typedata(rettype, record->getASTContext());
            if(!retdata.ok)
                continue;

            std::string stmtclose = ");";
            if(method->getKind() == Decl::Kind::CXXConstructor)
            {
                retdata.kind = Typedata::POINTER;
                retdata.ctype = cstructname+"*";
                retdata.forwardDecl = "struct ";
                // TODO, maybe set pointee value in retdata?

                cfuncbody << "return (" << retdata.forwardDecl
                    << retdata.ctype << ")new " << recordname << "(";

                // Has to have accompanying free function for the type
                auto freefuncsig = cstructname+("_free()");
                cfunccomment << ("// Return ptr must be manually freed with ") + freefuncsig + "\n";
            }
            else if(methodname == "operator=")
            {
                retdata = Typedata(retdata, Typedata::FUNDEMENTAL);
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

                    retdata = Typedata(retdata, Typedata::POINTER);
                    
                    cfuncbody << "return (" << retdata.ctype << ")new " << rettype.getAsString() << "(";
                    stmtclose = "));";

                    // Has to have accompanying free function for the type
                    auto freefuncsig = ToCIdentifier(rettype.getAsString())+("_free()");
                    cfunccomment << "// Return ptr must be manually freed with " << freefuncsig << "\n";
                }
            }
            else if(retdata.kind == Typedata::REFERENCE)
            {
                cfuncbody << "return (" << retdata.ctype << ")&";
            }
            else {
                // just return the value
                
                if(retdata.ctype != "void")
                    cfuncbody << "return ";
                cfuncbody << "(" << retdata.ctype << ")";
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
                logd("Static method");
                if(derived)
                    cfuncbody << derivedname << "::";
                else 
                    cfuncbody << recordname << "::";
            }
            else {
                std::string cparamtype = cstructname+"*";
                headerparams += ("struct ")+cparamtype+(" _instance");

                if(derived)
                {
                    cfuncbody << "static_cast<" << recordname << " *>((" << derivedname 
                        << " *)_instance)->" << methodname << "(";
                }
                else
                {
                    cfuncbody << "((" << recordname 
                        << " *)_instance)->" << methodname << "(";
                }

                // json
                cfuncjson["params"].push_back({
                    {"name", "_instance"},
                    {"type", cparamtype},
                    {"kind", "pointer"}
                });
            }

            // return json
            cfuncjson["returns"] = {
                {"type", retdata.ctype},
                {"kind", retdata.kind}
            };

            // Params
            bool badparams = false;
            std::string sourceparams;
            for(auto param : method->parameters())
            {
                auto paramname = param->getName().str();
                if(paramname.empty())
                    paramname = ("_p")+std::to_string(param->getFunctionScopeIndex());
                auto paramtype = param->getType().getCanonicalType()
                    .getUnqualifiedType();
                auto typedata = Typedata(paramtype, record->getASTContext());
                
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
                    sourceparams += ("*(")+paramtype.getAsString()+("*)")
                        +paramname;
                }
                else {
                    sourceparams += ("(")+paramtype.getAsString()+(")")
                        +paramname;
                }

                headerparams += typedata.forwardDecl + typedata.ctype
                    + (" ") + paramname;

                cfuncjson["params"].push_back({
                    {"name", paramname},
                    {"type", typedata.ctype},
                    {"kind", typedata.kind}
                });
            }

            if(badparams)
                continue;

            cfuncbody << sourceparams;



            // bool badparams = false;
            // std::string paramsSourceContent;
            // for(auto param : method->parameters())
            // {
            //     bool isFundamental = false;
            //     ParamData paramdata;

            //     std::function<bool(clang::QualType)> handleParam = [&](
            //         clang::QualType type
            //     ){
            //         auto cpptype = type.getAsString();

            //         if(cpptype.find("std::") != std::string::npos)
            //         {
            //             logd("// type is in STD\n");
            //             badparams = true;
            //             return false;
            //         }
            //         else if(type->isRValueReferenceType())
            //         {
            //             // cannot handle these yet
            //             logd("// is RValue reference\n");
            //             badparams = true;
            //             return false;
            //         }
            //         else if(type->isFunctionPointerType())
            //         {
            //             logd("// function pointer");
            //             paramdata.type.kind = "function pointer";
            //             paramdata.c = GetCFuncPtr(type);
            //             if(paramdata.c.empty())
            //             {
            //                 logd("// Bad funcptr\n");
            //                 badparams = true;
            //                 return false;
            //             }
            //             paramdata.type.c = std::regex_replace(paramdata.c, reg::fnptrname, ("(*")+paramdata.name+")");
            //             paramdata.sourceContent = ("(")+cpptype+(")")+paramdata.name;
            //         }
            //         else if(type->isFundamentalType() || type->isBooleanType())
            //         {
            //             logd("// fundemental type");

            //             isFundamental = true;

            //             paramdata.type.kind = "fundemental";
            //             paramdata.type.c = cpptype;
            //             paramdata.c = paramdata.type.c+(" ")+paramdata.name;
            //             paramdata.sourceContent = ("(")+cpptype+(")")+paramdata.name;
            //         }
            //         else if(type->isEnumeralType())
            //         {
            //             logd("// enumaral type");
            //             paramdata.type.kind = "enum";
            //             paramdata.type.c = ToCType(cpptype);
            //             // paramdata.type.forwardDecl = "enum ";
            //             paramdata.c = paramdata.type.c+(" ")+paramdata.name;
            //             paramdata.sourceContent = ("(")+cpptype+(")")+paramdata.name;
            //         }
            //         else if(type->isArrayType())
            //         {
            //             logd("// array type");
            //             logd("// " << type.getAsString());

            //             auto elemtype = type->getPointeeOrArrayElementType()
            //                 ->getLocallyUnqualifiedSingleStepDesugaredType();

            //             logd("// array element type: " << elemtype.getAsString());

            //             if(!handleParam(elemtype))
            //                 return false;

            //             if(paramdata.type.kind == "function pointer")
            //             {
            //                 std::cout << "don't know what to do when function pointer in array param handler!!" << std::endl;
            //                 // throw new std::logic_error("don't know what to do when function pointer in array param handler");
            //                 abort();
            //             }

            //             // get array size
            //             if(type->isVectorType())
            //             {
            //                 logd("vector type");
            //             }
            //             // comment
            //             paramdata.type.kind = "array";
            //             paramdata.type.c+="*";
            //             paramdata.c = paramdata.type.c+(" ")+paramdata.name;
            //             paramdata.sourceContent = ("(")+cpptype+(")")+paramdata.name;
            //         }
            //         else if(type->isPointerType() || type->isReferenceType())
            //         {
            //             logd("// Pointer type");

            //             paramdata.type.kind = "pointer";
            //             if(!handleParam(type->getPointeeType()))
            //                 return false;

            //             paramdata.type.c+="*";
            //             paramdata.c = paramdata.type.c+(" ")+paramdata.name;
            //             paramdata.sourceContent = ("(")+cpptype+(")")+paramdata.name;
            //         }
            //         else {
            //             // make it pointer type
                        
            //             paramdata.type.c = ToCType(cpptype);
            //             if(paramdata.type.kind != "pointer")
            //                 paramdata.type.c+=("*");
                            
            //             paramdata.type.kind = "pointer";
            //             if(isFundamental)
            //                 paramdata.c = paramdata.type.c+(" ")+paramdata.name;
            //             else
            //                 paramdata.c = ("struct ")+paramdata.type.c+(" ")+paramdata.name;

            //             paramdata.sourceContent = ("*(")+cpptype+("*)")+paramdata.name;
            //         }
                    
            //         return true;
            //     };

            //     // do streams
            //     if(!params.empty())
            //         params+=", ";
            //     if(!paramsSourceContent.empty())
            //         paramsSourceContent+=", ";

            //     auto paramtype = param->getType().getCanonicalType()
            //         .getUnqualifiedType();
            //     paramdata.name = param->getName().str();
            //     if(paramdata.name.empty())
            //     {
            //         logd("// param has no name");
            //         paramdata.name = "p"+std::to_string(param->getFunctionScopeIndex());
            //     }
            //     logd("// Param " << paramdata.name << ", CPP: " << paramtype.getAsString());

            //     if(!handleParam(paramtype))
            //         continue;

            //     params += paramdata.c;

            //     cfuncjson["params"].push_back({
            //         {"name", paramdata.name},
            //         {"type", paramdata.type.c},
            //         {"kind", paramdata.type.kind}
            //     });
            //     paramsSourceContent += paramdata.sourceContent;
            // }

            // if(badparams)
            //     continue;

            // cfuncsource << paramsSourceContent << ")";

            
            // else if(copyCtor)
            // {
            //     cfuncsource << "))";
            // }

            cfuncbody << stmtclose;

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
            capisource << funcsig 
                << "\n{\n    " << cfuncbody.str() << "\n}\n\n";

            // json
            cfuncjson["comment"] = cfunccomment.str();
            cfuncjson["struct"] = cstructname;
            capijson["functions"][cfuncname] = cfuncjson;
        }
    };

    // Methods
    if(
        record->getAccess() == AccessSpecifier::AS_none
            || record->getAccess() == AccessSpecifier::AS_public
    )
    {

        /* debug */
        auto ispublic = record->getAccess() == AccessSpecifier::AS_public;
        auto isnone = record->getAccess() == AccessSpecifier::AS_none;
        auto isvisible = record->getVisibility() == Visibility::DefaultVisibility;
        /* debug */

        auto describedRecord = dyn_cast_or_null<ClassTemplateDecl>(record);
        if(describedRecord) 
        {
            /* debug */
            auto ispublic = describedRecord->getAccess() == AccessSpecifier::AS_public;
            auto isnone = describedRecord->getAccess() == AccessSpecifier::AS_none;
            auto isvisible = describedRecord->getVisibility() == Visibility::DefaultVisibility;
            /* debug */
            logd("");
        }

        logd("// functions from " << recordnameorig);

        // free function
        if(record->hasSimpleDestructor())
        {
            // Destructor is accessible
            auto freefuncname = cstructname + ("_free");
            auto cfreefuncsig = ("CAPI void ") + freefuncname + ("(struct ") + cstructname + ("* ptr)");
            capiheader(cfreefuncsig << ";\n\n");
            capisource << cfreefuncsig << "\n{\n    delete (" 
                << recordnameorig << " *)ptr;\n}\n\n";

            if(funcnames.find(freefuncname) != funcnames.end())
                freefuncname+=("_")+std::to_string(funcnames[freefuncname]++);
            else {
                funcnames[freefuncname] = 1;
            }
        }
        else
        {
            logd("// has no accessible destructor for 'free' function\n");
        }

        domethods(record, nullptr);
    }
    else
    {
        logd("// the record and its methods are not publicly accessible\n");
    }

    // Add record to bookkept symbols
    capisymbols.emplace(cstructname);
    
    //record->dump(ast);
});
