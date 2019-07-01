/**
 * This clang tool is primarily made to process alt:V's cpp-sdk for CAPI
 */

#include "tool.hpp"

#define ASTFileName "altv-capi.ast"
#define LogFileName "altv-capi.log"
#define JsonFileName "altv-capi.json"
#define CHeaderFileName "altv-capi.h"
#define CPPHeaderFileName "altv-capi.hpp"
#define SourceFileName "altv-capi.cpp"

std::string cheaderstart = R"(
/**
 * CAPI API Header for C
 * THIS HEADER IS AUTOGENERATED
 * )" + GetDateAndTime() + R"(
 * 
 * Include header for alt:V C API
 */

#ifndef _CAPI_H_
#define _CAPI_H_

#include <stdbool.h>
#define CAPIEXTERN

#ifdef _WIN32
#ifdef CAPI_DLL
#define CAPI CAPIEXTERN __declspec(dllimport)
#else
#define CAPI CAPIEXTERN
#endif // CAPI_DLL
#else
#define CAPI CAPIEXTERN
#endif // _WIN32

)";

std::string cppheaderstart = R"(
/**
 * CAPI API Header for C++
 * THIS HEADER IS AUTOGENERATED
 * )" + GetDateAndTime() + R"(
 * 
 * Include header for alt:V C API
 */

#pragma once

#define _Bool bool
#define CAPIEXTERN extern "C"

#ifndef CAPI
#ifdef CAPI_DLL
#ifdef _WIN32
#define CAPI CAPIEXTERN __declspec(dllimport)
#endif // _WIN32
#else
#define CAPI CAPIEXTERN
#endif // CAPI_DLL
#endif // CAPI

)";

const char* cheaderend = R"(

#include "altv-capi-extra.h"

#endif // _CAPI_H_
)";

const char* cppheaderend = R"(

#include "altv-capi-extra.h"
)";

const char* sourcestart = R"(
/**
 * THIS HEADER IS AUTOGENERATED
 * )" __DATE__ " " __TIME__ R"(
 * 
 * Source implementation for alt:V C API
 */

#include "cpp-sdk/SDK.h"

#ifdef CAPI_DLL
#ifdef _WIN32
#define CAPI CAPIEXTERN __declspec(dllexport)
#endif // _WIN32
#else
#define CAPI CAPIEXTERN
#endif // CAPI_DLL
#include ")" CPPHeaderFileName R"("

using namespace alt;

)";

const char* sourceend = R"(

#include "altv-capi-extra.cpp"
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
std::ofstream capicppheader(CPPHeaderFileName);
std::ofstream capisource(SourceFileName);

// 
std::unordered_set<std::string> capisymbols;

/**
 * Fix some errors in the compiler invocation before sending it to clang.
 *
 * This is a hack! Some header search path by default is incorrect. It causes
 * libclang not able to find stddef.h. This method is only tested on Arch Linux.
 */
// void fix_invocation(CompilerInvocation& invoc) {
//     for (auto& entry : invoc.getHeaderSearchOpts().UserEntries) {
//         if (entry.IsInternal) {
//             if (!boost::filesystem::is_directory(entry.Path)) {
//                 entry.Path = "/usr/bin/" + entry.Path;
//             }
//         }
//     }
// }

/**
 * Get the AST unit from the files provided in the command line.
 */
std::unique_ptr<ASTUnit> get_ast_unit(int argc, const char **argv) {
    DiagnosticOptions diag_opts;
    diag_opts.ShowColors = true;

    auto diags = CompilerInstance::createDiagnostics(&diag_opts);

    std::vector<const char*> command_args_vector{
        "-std=c++17",
        "-x", "c++"
    };
    std::copy_n(argv + 1, argc - 1, std::back_inserter(command_args_vector));

    auto command_args = llvm::makeArrayRef(command_args_vector);
    auto invoc = clang::createInvocationFromCommandLine(command_args, diags);

    if (invoc == nullptr) {
        return nullptr;
    }

    //fix_invocation(*invoc);
    
    //PCHContainerOperations pchops;
    auto filemgr = new FileManager(invoc->getFileSystemOpts());
    auto gg = std::shared_ptr<PCHContainerOperations>(new PCHContainerOperations());
    auto gg2 = std::shared_ptr<CompilerInvocation>(invoc.release());
    auto raw_unit = ASTUnit::LoadFromCompilerInvocation(
        gg2,
        gg,
        diags,
        filemgr
    );
    return raw_unit;
}

std::unique_ptr<ASTUnit> unit;
Sema* semma;

int main(int argc, const char **argv)
{
    pp.SuppressTagKeyword = true;
    pp.FullyQualifiedName = true;

    cl::OptionCategory category("apt-tool options");
    CommonOptionsParser optionsParser(argc, argv, category, 0);

    // Get Sema
    // unit = get_ast_unit(argc, argv);
    // semma = &unit->getSema();

    std::cout << "Generating CAPI..." << std::endl;

    // Header/Source start
    capixheader(cheaderstart, cppheaderstart);
    capisource << sourcestart;

    // Run handlers
    ClangTool tool(optionsParser.getCompilations(),
                    optionsParser.getSourcePathList());
    Handler::ExecuteAll(tool);
    
    // Header/Source end
    capixheader(cheaderend, cppheaderend);
    capisource << sourceend;

    capicheader.close();
    capicppheader.close();
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
