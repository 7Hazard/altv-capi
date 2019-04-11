#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "Info.hpp"

namespace capi {

class Function
{
public:
    bool ok;

    Function(clang::CXXMethodDecl* method)
    {
        if (IsMethodInvalid(method)) continue;

        auto& astcontext = method->getASTContext();
        auto& sourcemanager = astcontext.getSourceManager();

        auto rc = astcontext.getRawCommentForDeclNoCache(method);
        auto methodcomment = rc ? rc->getRawText(sourcemanager).str() : "/// No comment";

        auto methodreturns = Type::Get(method->getReturnType());
        auto methodcname = ToCFormat(method->getNameAsString());
        auto methodisstatic = method->isStatic();

        funcdecls
            << methodcomment << std::endl
            << "CAPI "
            << methodreturns.getAsString() << " " 
            << recordcname << "_" // Class name as method prefix
            << methodcname << "();"
            << std::endl << std::endl;
    }
};

}
