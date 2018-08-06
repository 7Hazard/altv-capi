/**
 * This is a clang tool primarily made to parse and work with alt:V's cpp-api
 */

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;
using nlohmann::json;

#define OUTPUT_NAME "cpp-api-info.json"

// Tool info
static cl::OptionCategory category("apt-tool options");
static cl::extrahelp commonHelp(CommonOptionsParser::HelpMessage);
// static cl::extrahelp moreHelp("\nMore help text...");

LangOptions lo;
PrintingPolicy pp(lo);

json classes;

// CXXRecordDecl are structs/classes/unions
DeclarationMatcher recordMatcher = cxxRecordDecl().bind("records");

// Checks if the record is local to the processed files
bool IsRecordLocal(const CXXRecordDecl *record) {
  FullSourceLoc fullSourceLoc(record->getLocation(),
                              record->getASTContext().getSourceManager());
  return fullSourceLoc.isValid() && !fullSourceLoc.isInSystemHeader();
}

class RecordHandler : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    const CXXRecordDecl *rd =
        Result.Nodes.getNodeAs<clang::CXXRecordDecl>("records");

    // Only handle local records
    if (!IsRecordLocal(rd) || rd->isTemplated() || !rd->isThisDeclarationADefinition())
      return;

    json methods;

    // Iterate over record's methods
    for (auto method : rd->methods()) {
      // Templates are not fun
      if (method->isTemplated() ||
          method->getVisibility() != clang::Visibility::DefaultVisibility)
        continue;

      // Get method params
      json params;
      for (auto param : method->parameters()) {
        auto type = param->getType();
        params.push_back({
            {"name", param->getNameAsString()},
            {"type", type->isBooleanType() ? "bool" : type.getAsString()},
            {"baseType", type.getNonReferenceType().getUnqualifiedType().getAsString(pp)},
            {"isPtr", type->isPointerType()},
            {"isRef", type->isReferenceType()},
            {"isConst", type.isConstQualified()},
            //{"qualifiers", type.getQualifiers().getAsString()},
        });
      }

      // Set current method info to record methods
      auto returnType = method->getReturnType();
      methods.push_back({
          {"name", method->getNameAsString()},
          {"returns", {
            {"type", returnType->isBooleanType() ? "bool" : returnType.getAsString()},
            {"baseType", returnType.getNonReferenceType().getUnqualifiedType().getAsString(pp)},
            {"isPtr", returnType->isPointerType()},
            {"isRef", returnType->isReferenceType()},
            {"isConst", returnType.isConstQualified()},
            //{"qualifiers", type.getQualifiers().getAsString()},
          }},
          {"params", params},
          {"isCtor", method->getKind() == Decl::CXXConstructor},
          {"isDtor", method->getKind() == Decl::CXXDestructor},
          {"isStatic", method->isStatic()},
      });
    }

    // Set the JSON info
    classes.push_back({
        {"name", rd->getNameAsString()},
        {"methods", methods},
    });
  }
};

int main(int argc, const char **argv) {
  pp.SuppressTagKeyword = true;

  CommonOptionsParser optionsParser(argc, argv, category, 0);

  ClangTool tool(optionsParser.getCompilations(),
                 optionsParser.getSourcePathList());

  RecordHandler printer;
  MatchFinder finder;
  finder.addMatcher(recordMatcher, &printer);

  std::cout << "Processing API..." << std::endl;
  tool.run(newFrontendActionFactory(&finder).get());

  std::ofstream out(OUTPUT_NAME);
  out << std::setw(4) << classes;
  std::cout << "Output to " << OUTPUT_NAME << std::endl;
  return 0;
}
