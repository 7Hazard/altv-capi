#include "tool.hpp"

static auto matcher = cxxMethodDecl(

    unless(isExpansionInSystemHeader())
).bind("method");

/*
static Handler h(matcher, [](const MatchFinder::MatchResult& result){
    auto method = result.Nodes.getNodeAs<CXXMethodDecl>("method");

    auto methodname = method->getQualifiedNameAsString();

    //logfile << "Method: " << methodname << std::endl;

#ifdef _DEBUG
    auto loc = method->getLocation().printToString(*result.SourceManager);
    capiheader 
        << "// Method" << std::endl
        << "// " << loc << std::endl
        << "// " << methodname << std::endl;
#endif

    capiheader << "\n" << std::endl;
});
*/
