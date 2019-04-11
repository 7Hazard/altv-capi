/**
 * This is a clang tool primarily made to parse and work with alt:V's cpp-sdk
 */

#include "Info.hpp"

// Config

auto Info::ignoredRecords = {
    "dummy",
    //"alt::String",
    //"alt::StringView",
    //"alt::Array"
};

auto Info::ignoredPaths = {
    "dummy",
    "cpp-sdk/deps"
};

// Config

#include <filesystem>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

namespace fs = std::experimental::filesystem;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;
using nlohmann::json;

#define OUTPUT_NAME "capi.json"

//
std::string rootFolder = "./";
std::ofstream funcdecls("include/altv/capi-funcs-auto.h");
std::ofstream typedecls("include/altv/capi-types-auto.h");

// Clang global stuff
LangOptions lo;
PrintingPolicy pp(lo);
static cl::OptionCategory category("apt-tool options");
static cl::extrahelp commonHelp(CommonOptionsParser::HelpMessage);
// static cl::extrahelp moreHelp("\nMore help text...");

#include "utils.hpp"
#include "Type.hpp"
#include "Struct.hpp"
#include "Function.hpp"

// CXXRecordDecl are structs/classes/unions
DeclarationMatcher recordMatcher = cxxRecordDecl().bind("records");

// debug?
std::string aststr;
raw_string_ostream ast(aststr);
std::ofstream astOut("cpp-api.ast");
// debug?

class RecordHandler : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const CXXRecordDecl *rd = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("records");

        if(rd->isStruct() || rd->isClass())
        {
            capi::Struct s(rd);
        }
        else throw new std::exception("Unhandled record");

        //auto recordname = rd->getQualifiedNameAsString();
        //auto invalid = IsRecordInvalid(rd);
        //if (invalid) return;
//
        //auto recordcname = ToCFormat(recordname);
//
        //// All bases
        //for(auto b : rd->bases())
        //{
        //    for (auto method : rd->methods()) {
        //        if (IsMethodInvalid(method)) continue;
//
        //    }
        //}
//
        //// Handle methods
        //for (auto method : rd->methods()) {
        //    auto func = capi::Function(method);
        //    if (!func.ok) continue;
//
//
        //}
//
        //// In-class declerations
        //for(auto decl : rd->decls())
        //{
//
        //}
    }
};

int main(int argc, const char **argv)
{
    fs::create_directories("include/altv");

    auto fileheader =
R"(
/**
 * THIS FILE IS AUTO-GENERATED
 */

)";
    funcdecls << fileheader;
    typedecls << fileheader;

    pp.SuppressTagKeyword = true;
    pp.FullyQualifiedName = true;

    CommonOptionsParser optionsParser(argc, argv, category, 0);

    ClangTool tool(optionsParser.getCompilations(),
                    optionsParser.getSourcePathList());

    RecordHandler recordHandler;
    MatchFinder finder;
    finder.addMatcher(recordMatcher, &recordHandler);

    std::cout << "Processing..." << std::endl;
    tool.run(newFrontendActionFactory(&finder).get());

    // capi info
    std::ofstream out(OUTPUT_NAME);
    //out << std::setw(4) << classes;
    std::cout << "Output " << OUTPUT_NAME << std::endl;
    
    return 0;
}
