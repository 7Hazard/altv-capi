#include "tool.hpp"

static auto templateClassMatcher = classTemplateSpecializationDecl(

    unless(isExpansionInSystemHeader())

    //,hasSpecializedTemplate(classTemplateDecl())
    //,hasAnyTemplateArgument(anything())

    ,unless(has(classTemplatePartialSpecializationDecl()))
    ,unless(has(classTemplateDecl()))
    ,unless(has(templateTypeParmType()))
    // ,isDefinition()
).bind("template class");

extern clang::CompilerInstance* comp;

static Handler templateClassHandler(templateClassMatcher, [](const MatchFinder::MatchResult& result){
    auto temprecord = result.Nodes.getNodeAs<ClassTemplateSpecializationDecl>("template class");

    auto qualType = temprecord->getTypeForDecl()->getLocallyUnqualifiedSingleStepDesugaredType();
    auto recordnameorig = qualType.getAsString();
    std::string cstructname = ToCIdentifier(recordnameorig);

    logd("// TEMPLATE CLASS: " << recordnameorig << " | CAPI: " << cstructname << std::endl);

    if(temprecord->getDeclKind() == Decl::Kind::ClassTemplate)
    {
        logd("// Is ClassTemplate.\n");
        return;
    }
    else if(temprecord->isTemplated() || temprecord->isTemplateDecl())
    {
        logd("// isTemplated || isTemplateDecl.\n");
        return;
    }
    
    // Dont process if already defined
    if(SymbolDefined(cstructname))
    {
        logd("// symbol already defined");
        return;
    }

    // Only need to complete the type, the other matchers will output the fields and methods
    if(comp->getSema().RequireCompleteType(temprecord->getLocation(), qualType, DiagnosticIDs::Warning) != false)
    {
        logd("// could not get complete type \n");
        return;
    }
    auto record = temprecord;
});
