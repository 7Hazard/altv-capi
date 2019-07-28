
#include "tool.hpp"

clang::AccessSpecifier currentAccess = AccessSpecifier::AS_none;

static auto matcher = recordDecl(
    hasDescendant(
        accessSpecDecl().bind("access")
    )
).bind("record");

static Handler handler(matcher, [](const MatchFinder::MatchResult& result){
    auto access = result.Nodes.getNodeAs<AccessSpecDecl>("access");

    currentAccess = access->getAccess();
});
