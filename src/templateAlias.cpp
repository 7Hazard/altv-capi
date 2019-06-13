#include "tool.hpp"

static auto templateAliasMatcher = typeAliasTemplateDecl(

    unless(isExpansionInSystemHeader())
).bind("template alias");

/**
 * Instantiate a template alias (`template <...> using Foo = ...`).
 */
TypeAliasDecl* instantiate(ASTContext& ast, Sema& sema, DeclContext* parent,
                           TypeAliasTemplateDecl* decl,
                           ArrayRef<TemplateArgument> args) {
    auto args_count = static_cast<unsigned>(args.size());
    TemplateArgumentList arg_list {TemplateArgumentList::OnStack, args};
    MultiLevelTemplateArgumentList multi_arg_list {arg_list};
    TemplateDeclInstantiator instantiator {sema, parent, multi_arg_list};
    auto instantiated = instantiator.Visit(decl);
    if (auto inst_decl = dyn_cast<TypeAliasTemplateDecl>(instantiated)) {
        return inst_decl->getTemplatedDecl();
    }
    return nullptr;
}

static Handler templateAliasHandler(templateAliasMatcher, [](const MatchFinder::MatchResult& result){
    auto alias = result.Nodes.getNodeAs<TypeAliasTemplateDecl>("template alias");
    auto aliastype = alias->getUnderlyingDecl();

    // Data
    auto aliasnameorig = alias->getQualifiedNameAsString();
    std::string aliasname = ToCIdentifier(aliasnameorig);

    auto loc = alias->getLocation().printToString(*result.SourceManager);
#ifdef _DEBUG
    logd( 
        "// Template Alias" << std::endl
        << "// " << loc << std::endl
        << "// " << aliasnameorig << " = " << aliastype->getQualifiedNameAsString()
    );
#endif
    auto kind = alias->getKind();

    if(!alias->isCanonicalDecl())
    {
        logd("// Not canonical");
    }
    
    //alias->init(alias->getTemplatedDecl(), alias->getTemplateParameters());

    //auto t = instantiate(alias->getASTContext(), );
    //alias->getCanonicalDecl()->getDescribedTemplate()
    //    ->init(alias->getTemplatedDecl(), alias->getTemplateParameters());

    capiheader << std::endl;
});
