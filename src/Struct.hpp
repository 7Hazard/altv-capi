#pragma once

#include <string>

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "Info.hpp"
#include "Type.hpp"

namespace capi
{

class Struct : Type
{
public:
    struct Field
    {
        std::string name;
        Type* type;
        nlohmann::json json;

        Field(clang::FieldDecl* decl)
        {
            name = decl->getName().str();
            type = Type::Get(decl->getType());
        }
    };

    //clang::CXXRecordDecl* record;
    std::vector<Field> fields;

    Struct(const clang::CXXRecordDecl* rd)
    {
        // Set base data
        kind = "Struct";
        name = rd->getQualifiedNameAsString();

        // Validate struct
        for (auto v : Info::ignoredRecords)
        {
            if (name == v)
            {
                ok = false;
                return;
            }
        }

        // Fields
        for(auto field : rd->fields())
        {
            fields.emplace_back(field);

            //auto& astcontext = field->getASTContext();
            //auto& sourcemanager = astcontext.getSourceManager();
//
            //auto rc = astcontext.getRawCommentForDeclNoCache(field);
            //auto fieldcomment = rc ? rc->getRawText(sourcemanager).str() : "/// No comment";
//
            //auto fieldtype = field->getType();
            //auto fieldname = field->getName().str();
//
            //typedecls
            //    << fieldcomment << std::endl
            //    << fieldtype.getAsString(pp) << " "
            //    << fieldname
            //    << std::endl << std::endl;
        }

        // Prepare json data

        // Cache
        Cache();
    }
};

}
