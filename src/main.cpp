/**
 * This clang tool is primarily made to process alt:V's cpp-sdk for CAPI
 */

#include "tool.hpp"

#define ASTFileName "altv-capi.ast"
#define LogFileName "altv-capi.log"
#define JsonFileName "altv-capi.json"
#define CHeaderFileName "altv-capi.h"
#define SourceFileName "altv-capi.cpp"

std::string cheaderstart = R"(
/**
 * CAPI API Header for C
 * THIS HEADER IS AUTOGENERATED
 * )" + GetDateAndTime() + R"(
 * 
 * Include header for alt:V C API
 */

#ifndef _ALTV_CAPI_H_
#define _ALTV_CAPI_H_

)";

const char* cheaderend = R"(

#endif // _ALTV_CAPI_H_
)";

std::string sourcestart = R"(
/**
 * THIS HEADER IS AUTOGENERATED
 * )" + GetDateAndTime() + R"(
 * 
 * Source implementation for alt:V C API
 */


using namespace alt;

)";

const char* sourceend = R"(

)";

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/Utils.h>

LangOptions lo;
PrintingPolicy pp(lo);

// AST
std::error_code ec;
raw_fd_ostream ast(ASTFileName, ec);

// Log
std::ofstream logfile(LogFileName);

// JSON
json capijson = json::object();

// Header/Source outputs
std::ofstream capicheader(CHeaderFileName);
std::ofstream capisource(SourceFileName);

// 
std::unordered_set<std::string> capisymbols;

CompilerInstance* comp;

void Handler::ExecuteAll(int argc, const char **argv)
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

    auto actionfac = newFrontendActionFactory(&finder);
    
    DiagnosticOptions* diag_opts = new DiagnosticOptions();
    diag_opts->ShowColors = true;

    auto diags = CompilerInstance::createDiagnostics(diag_opts);

    std::vector<const char*> command_args_vector;
    std::copy_n(argv, argc, std::back_inserter(command_args_vector));

    auto command_args = llvm::makeArrayRef(command_args_vector);
    auto invoc = clang::createInvocationFromCommandLine(command_args, diags);

    if (invoc == nullptr) {
        return;
    }
    
    auto filemgr = new FileManager(invoc->getFileSystemOpts());
    auto compilerInvocation = std::shared_ptr<CompilerInvocation>(invoc.release());
    auto pch = std::shared_ptr<PCHContainerOperations>(new PCHContainerOperations());

    CompilerInstance compiler(std::move(pch));
    compiler.setInvocation(std::move(compilerInvocation));
    compiler.setFileManager(filemgr);
    compiler.setDiagnostics(diags.get());

    auto action = actionfac->create();
    comp = &compiler;
    if(!compiler.ExecuteAction(*action))
    {
        std::cout << "ACTION NOT SUCCESS" << std::endl;
        return;
    }
}

cl::OptionCategory category("capi generator options");

// BUGGED ON LINUX, MAYBE CAUSE OF MULTIPLE INSTALLED VERSIONS OF LLVM/CLANG
// llvm::cl::opt<bool> debugEnabled(
//     "debug",
//     cl::desc("Outputs debug info in headers"),
//     cl::init(false), cl::cat(category)
// );

bool isDebugEnabled()
{
    // return debugEnabled.getValue();
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

int main(int argc, const char **argv)
{
    pp.SuppressTagKeyword = true;
    pp.FullyQualifiedName = true;

    CommonOptionsParser optionsParser(argc, argv, category, 0);

    std::set<std::string> compilerOptions;
    for (const auto& file : optionsParser.getSourcePathList()) {
        auto Commands = optionsParser.getCompilations().getCompileCommands(file);
        for (auto opt : Commands[0].CommandLine) {
            compilerOptions.emplace(opt);
        }
    }
    std::vector<const char*> compilerOptionsCStr;
    for(auto opt : compilerOptions)
    {
        auto str = new char[opt.length()];
        strcpy(str, opt.c_str());
        compilerOptionsCStr.push_back(str);
    }
    {
        // auto g = *(compilerOptionsCStr.end() - 1);
        // delete g;
        compilerOptionsCStr.erase(compilerOptionsCStr.end() - 1);
    }

    std::cout << "Generating CAPI..." << std::endl;

    // Header/Source start
    capiheader(cheaderstart);
    capisource << sourcestart;

    // Run handlers
    // ClangTool tool(optionsParser.getCompilations(),
    //                 optionsParser.getSourcePathList());
    // Handler::ExecuteAll(tool);
    Handler::ExecuteAll(compilerOptionsCStr.size(), compilerOptionsCStr.data());
    
    // Header/Source end
    capiheader(cheaderend);
    capisource << sourceend;

    capicheader.close();
    std::cout << "Output " CHeaderFileName << std::endl;

    ast.close();
    std::cout << "Output " ASTFileName << std::endl;

    std::ofstream out(JsonFileName);
    out << std::setw(4) << capijson;
    out.close();
    std::cout << "Output " JsonFileName << std::endl;

    logfile.close();
    std::cout << "Output " LogFileName << std::endl;

    return 0;
}
