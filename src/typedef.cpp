#include "tool.hpp"

static auto typedefMatcher = typedefNameDecl(
    unless(isExpansionInSystemHeader())

    //,unless(has(typeAliasTemplateDecl()))
    //,unless(has(templateSpecializationType()))
    ,unless(has(templateTypeParmType()))
    //,unless(has(templateTypeParmDecl()))
    //,unless(has(classTemplateDecl()))
    //,unless(has(classTemplatePartialSpecializationDecl()))
    ,unless(classTemplatePartialSpecializationDecl())
).bind("typedef");

static Handler typedefHandler(typedefMatcher, [](const MatchFinder::MatchResult& result){
    auto td = result.Nodes.getNodeAs<TypedefNameDecl>("typedef");

    // Data
    auto tdnameorig = td->getQualifiedNameAsString();
    std::string tdname = ToCIdentifier(tdnameorig);
    auto srcloc = td->getLocation();
    auto loc = srcloc.printToString(*result.SourceManager);
    logd("// Typedef");
    logd("// " << loc);
    logd("// " << tdnameorig);

    logd("// Canonical decl: " << td->getCanonicalDecl()->getQualifiedNameAsString());
    if(td->hasBody())
    {
        logd("// Has Body");   
    }
    if(td->hasDefiningAttr())
    {
        logd("// Has Defining attribute");
    }
    auto tdtd = td->getTypeForDecl();
    if(tdtd)
    {
        logd("// tdtd");
        if(tdtd->isUndeducedType())
        {
            logd("// tdtd undeduced");
        }
        if(tdtd->isIncompleteType())
        {
            logd("// tdtd incomplete type");
        }
        if(tdtd->isInstantiationDependentType())
        {
            logd("// tdtd instantiation dependent");
        }

        auto tdstruct = tdtd->getAsStructureType();
        if(tdstruct)
        {
            logd("// tdstruct");
        }
    }

    auto anondecl = td->getAnonDeclWithTypedefName();
    if(anondecl)
    {
        logd("// Has anondecl");
        if(anondecl->hasBody())
        {
            logd("// anondecl has body");
        }

        if(anondecl->isCompleteDefinition())
        {
            logd("// anondecl Is complete definition");
        }

        auto anondef = anondecl->getDefinition();
        if(anondef)
        {
            logd("// anondef exists");
        }
    }

    auto describedTemplate = td->getDescribedTemplate();
    if(describedTemplate)
    {
        logd("// Describes template");
        if(describedTemplate->hasDefiningAttr())
        {
            logd("// Described template has defining attribute");
        }
    }

    auto tdtype = td->getUnderlyingType().getCanonicalType();
    std::string ctddef;

    logd("// using " << tdnameorig << " = " << tdtype.getAsString());
  
    if(loc == "<invalid loc>") 
    {
        logd("// Invalid loc\n");
        return;
    }

    /*
    //else if(!td->isUsed())
    //{
    //    logd("// Is never used\n");
    //    return;
    //}
    else if(tdtype->isRecordType())
    {
        auto record = tdtype->getAsCXXRecordDecl();
        ast << "TYPEDEF CLASS " << tdnameorig << " = " << record->getQualifiedNameAsString() << "\n";
        astdump(record);

        //auto und = td->getUnderlyingType();
        //ast << "Typedef underlying type" << '\n';
        //ast << ToCType(und.getAsString()) << '\n';
        //astdump(und);
        //
        //auto undrec = und->getAsRecordDecl();
        //ast << "Typedef underlying type rec" << '\n';
        //ast << ToCType(undrec->getQualifiedNameAsString()) << '\n';
        //astdump(undrec);
        //
        //logd("// " << ToCType(tdtype.getAsString()));
        //
        //if(record->hasDefinition())
        //{
        //    capicheader << GetCRecord(result, record) << std::endl;
        //}
        //
        //logd("// " << record->getQualifiedNameAsString());
        //logd("// " << ToCType(tdtype.getAsString()));
        //
        //ast << "Typedef" << '\n';
        //ast << ToCType(tdtype.getAsString()) << '\n';
        //astdump(record);

        
        // auto t = dyn_cast_or_null<ClassTemplateSpecializationDecl>(record);
        // if(t == nullptr)
        // {
        //     logd("// t was null");
        //     ast << "NOT ClassTemplateSpecializationDecl \n";
        //     return;
        // }

        // bool is_incomplete = semma->RequireCompleteType(
        //     t->getLocation(),
        //     t->getASTContext().getTypeDeclType(t),
        //     diag::err_unable_to_make_temp
        // );

        // if(!t->isExplicitInstantiationOrSpecialization())
        //     logd("// Not isExplicitInstantiationOrSpecialization");
        // if(!t->isExplicitSpecialization())
        //     logd("// Not isExplicitSpecialization");
        // if(!t->hasBody())
        // {
        //     logd("// Has no body");
        // }
        // if(!t->isCompleteDefinitionRequired())
        // {
        //     logd("// Complete definition not required");
        // }
        // if(!t->isCompleteDefinition())
        // {
        //     logd("// Is not complete definition");
        // }
        // if(!t->hasDefinition())
        // {
        //     logd("// Has no definition");
        // }
        
        
        //auto pinst = t->getPointOfInstantiation();
        //if(!pinst.isValid())
        //{
        //    logd("// No valid point of instantiation");
        //}

        //auto temppattern = record->getTemplateInstantiationPattern();



        
        // auto classTempSpec = dyn_cast_or_null<ClassTemplateSpecializationDecl>(record);
        // auto descClass = classTempSpec->getSpecializedTemplate();

        // //auto instantiatedClass = ClassTemplateSpecializationDecl::Create(
        // //    unit->getASTContext(),
        // //    TTK_Class,
        // //    (DeclContext*)td->getDeclContext(),
        // //    td->getLocation(),
        // //    td->getLocation(),
        // //    descClass,
        // //    classTempSpec->getTemplateInstantiationArgs().asArray(),
        // //    nullptr
        // //);
        // auto ok = semma->InstantiateClassTemplateSpecialization(
        //     td->getLocation(),
        //     classTempSpec,
        //     TemplateSpecializationKind::TSK_ExplicitInstantiationDefinition
        // );
        // if(!ok)
        // {
        //     return;
        // }
        // auto instantiatedClass = classTempSpec;

        // if(instantiatedClass)
        // {
        //     descClass->AddSpecialization(instantiatedClass, nullptr);

        //     auto typ = unit->getASTContext().getTypeDeclType(instantiatedClass);
        //     bool is_incomplete = semma->RequireCompleteType(
        //         instantiatedClass->getLocation(),
        //         typ,
        //         diag::err_unable_to_make_temp
        //     );

        //     if(is_incomplete)
        //     {
        //         logd("// Could not complete instantiated type");
        //         return;
        //     }
            

        //     logd("// INSTANTIATED CLASS");
        //     ast << "FROM SPECIALIZATION " << tdnameorig << "\n";
        //     astdump(classTempSpec);
        //     ast << "INSTANTIATED CLASS " << instantiatedClass->getQualifiedNameAsString() << "\n";
        //     astdump(instantiatedClass);

        //     auto t = instantiatedClass;
        //     if(!t->isExplicitInstantiationOrSpecialization())
        //         logd("// Not isExplicitInstantiationOrSpecialization");
        //     if(!t->isExplicitSpecialization())
        //         logd("// Not isExplicitSpecialization");
        //     if(!t->hasBody())
        //     {
        //         logd("// Has no body");
        //     }
        //     if(!t->isCompleteDefinitionRequired())
        //     {
        //         logd("// Complete definition not required");
        //     }
        //     if(!t->isCompleteDefinition())
        //     {
        //         logd("// Is not complete definition");
        //     }
        //     if(!t->hasDefinition())
        //     {
        //         logd("// Has no definition");
        //     }
        // }
        // else {
        //     logd("// COULD NOT INSTANTIATE CLASS");
        // }
        


        //ast << "Typedef ClassTemplateSpecializationDecl" << '\n';
        //ast << ToCType(tdtype.getAsString()) << '\n';
        //astdump(t);
        
        // first decl
        //if(!t->isFirstDecl())
        //{
        //    logd("// Is not first decl");
//
        //    auto ft = t->getFirstDecl();
//
//
        //    ast << "Typedef First Decl ClassTemplateSpecializationDecl" << '\n';
        //    ast << ToCType(tdtype.getAsString()) << '\n';
        //    astdump(ft);
        //}

        // redecls
       //for(auto redecl : t->redecls())
       //{
       //    ast << "Typedef Redecl ClassTemplateSpecializationDecl" << '\n';
       //    ast << ToCType(tdtype.getAsString()) << '\n';
       //    astdump(redecl);
       //}

        //ClassTemplateSpecializationDecl::Create(
        //    record->getASTContext(),
        //    TagDecl::TagKind::TTK_Class,
        //    record->getDeclContext(),
        //    srcloc,
        //    srcloc,
        //    record->getDescribedClassTemplate(),
        //    record->template
        //    nullptr
        //)
        //record->isCompleteDefinition();

        //auto desc = record->getDescribedClassTemplate();
        //if(desc)
        //{
        //    desc->init(record->template, desc->getTemplateParameters());
        //}
        //else
        //{
        //    logd("// No described template");
        //}
        
        
        //if(    
        //    record->getDeclKind() == Decl::Kind::ClassTemplateSpecialization
        //    //&& (record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ExplicitInstantiationDeclaration
        //    //|| record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ExplicitInstantiationDefinition
        //    //|| record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ExplicitSpecialization
        //    //|| record->getTemplateSpecializationKind() == TemplateSpecializationKind::TSK_ImplicitInstantiation)
        //)
        //{
        //    logd("// Specialized Record\n");
        //    capicheader << GetCRecord(result, record) << std::endl;
        //}
        //else
        //{
        //    logd("// Not template instanciated record\n");
        //}

        // I really have no idea, just make it struct
        tdname = ToCIdentifier(tdtype.getAsString());
        ctddef = ("struct ")+tdname+" "+tdname;
    }
    else if(!tdtype.isTrivialType(*result.Context)) 
    {
        logd("// Not trivial type\n");
        return;
    }
    else if(tdtype->isFunctionPointerType())
    {
        ctddef = GetCFuncPtr(tdtype);
        if(ctddef.empty())
        {
            logd("// Bad funcptr\n");
            return;
        }
    }
    else if(tdtype->isFundamentalType() || tdtype->isBooleanType()
        || (tdtype->isPointerType() && tdtype->getPointeeType()->isFundamentalType())
        )
    {
        ctddef = tdtype.getAsString()+(" ")+tdname;
    }
    else if(tdtype->isEnumeralType())
    {
        ctddef = ("enum ")+ToCType(tdtype.getAsString())+(" ")+tdname;
    }
    else {
        if(tdtype->isPointerType())
            ctddef = ("struct ")+ToCType(tdtype.getAsString())+(" ")+tdname;
        else
            ctddef = ("struct ")+ToCIdentifier(tdtype.getAsString())+(" ")+tdname;
    }

    if(capisymbols.find(tdname) != capisymbols.end())
    {
        logd("// " << tdname << " is already typedefed");
    }
    else {        
        capisymbols.emplace(tdname);
        capiheader("typedef " << ctddef << ";\n");
    }
    */

    auto typedata = Typedata(tdtype, td->getASTContext());
    if(!typedata.ok)
    {
        logd("// bad typedef\n");
        return;
    }
    else if(typedata.kind == Typedata::FUNCTION_POINTER)
    {
        auto tdef = typedata.forwardDecl + typedata.ctype;
        tdef = std::regex_replace(tdef, reg::fnptrname, ("(*")+tdname+")");

        capiheader(
            "typedef "
            << tdef
            << ";" << std::endl << std::endl
        );
    }
    else {
        capiheader(
            "typedef "
            << typedata.forwardDecl << typedata.ctype
            << " " << tdname
            << ";" << std::endl << std::endl
        );
    }

    //td->dump(ast);
});
