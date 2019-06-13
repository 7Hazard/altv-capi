#pragma once

#define ASTFileName "altv-capi.ast"
#define LogFileName "altv-capi.log"
#define JsonFileName "altv-capi.json"
#define HeaderFileName "altv-capi.h"
#define SourceFileName "alt-capi.cpp"

#define STRINGIFY(x) #x
#define astdump(node) node->dump(ast); ast << "\n--- END OF NODE ---\n\n"
#define sastdump(node) static bool dump_##node = false; \
    if(!dump_##node) { node->dump(ast); ast << "\n--- END OF NODE ---\n\n"; dump_##node = true; } 

#ifdef _DEBUG
#define logd(x) capiheader << x << std::endl
#else
#define logd(x)
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

#include "nlohmann/json.hpp"
using nlohmann::json;

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/Sema/Template.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

extern PrintingPolicy pp;
extern std::error_code ec;
extern std::unique_ptr<ASTUnit> unit;
extern Sema* semma;

extern raw_fd_ostream ast;
extern std::ofstream logfile;
extern json capijson;
extern std::ofstream capiheader;
extern std::ofstream capisource;
extern std::unordered_set<std::string> typedefs;

class Handler : public MatchFinder::MatchCallback
{
public:
    using HandlerFn = std::function<void(const MatchFinder::MatchResult&)>;

    StatementMatcher* stmtmatcher;
    DeclarationMatcher* declmatcher;
    HandlerFn fn;

    static inline std::vector<Handler*>& Handlers()
    {
        static std::vector<Handler*> handlers;
        return handlers;
    }

    static void ExecuteAll(ClangTool& tool)
    {
        std::cout << "Executing handlers..." << std::endl;
        std::cout << "Amount of handlers: " << Handlers().size() << std::endl;

        MatchFinder finder;

        for(auto handler : Handlers())
        {
            if(handler->stmtmatcher)
                finder.addMatcher(*handler->stmtmatcher, handler);
            else if(handler->declmatcher)
                finder.addMatcher(*handler->declmatcher, handler);
        }

        auto action = newFrontendActionFactory(&finder);
        tool.run(action.get());
        
    }

    Handler(StatementMatcher& matcher, HandlerFn fn)
        : stmtmatcher(&matcher), fn(fn)
    {
        Handlers().push_back(this);
    }

    Handler(DeclarationMatcher& matcher, HandlerFn fn)
        : declmatcher(&matcher), fn(fn)
    {
        Handlers().push_back(this);
    }

    virtual void run(const MatchFinder::MatchResult &Result)
    {
        fn(Result);

        logfile << std::endl;
    }
};


#include "util.hpp"
