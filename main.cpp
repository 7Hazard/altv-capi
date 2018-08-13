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

// Clang global stuff
LangOptions lo;
PrintingPolicy pp(lo);
static cl::OptionCategory category("apt-tool options");
static cl::extrahelp commonHelp(CommonOptionsParser::HelpMessage);
// static cl::extrahelp moreHelp("\nMore help text...");

// Utils
json merge(const json &a, const json &b) {
    json result = a;
    json tmp = b;
    for (json::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        result[it.key()] = it.value();
    }
    return result;
}

// Checks if the record is local to the processed files
bool IsRecordLocal(const CXXRecordDecl *record) {
    FullSourceLoc fullSourceLoc(record->getLocation(),
                                record->getASTContext().getSourceManager());
    return fullSourceLoc.isValid() && !fullSourceLoc.isInSystemHeader();
}

json GetFunctionPtrInfo(QualType type) {
    bool isFuncPtr = false;
    if (false) {
    }
    return {{"isFuncPtr", false}};
}

json GetTypeInfo(QualType type, std::string name = "") {
    return {
        {"name", name},
        {"type", type.getAsString()},
        {"baseType",
        type.getNonReferenceType().getUnqualifiedType().getAsString(pp)},
        //{"qualifiers", type.getQualifiers().getAsString()},

        {"isFundemental", type->isFundamentalType()},
        {"isElaborated", type->isElaboratedTypeSpecifier()},
        //{"isConst", type.isConstQualified()}, // idk why it doesnt work

        // Pointer stuff
        {"isPtr", type->isPointerType()},
        {"isRef", type->isReferenceType()},
        {"pointeeType", type->isPointerType() || type->isReferenceType() ? type->getPointeeType().getAsString() : ""},
        //{"funcPtrInfo", GetFunctionPtrInfo(type)},
        //{"isFuncPtr", type->isFunctionPointerType()},
        //{"isFunctionProtoType", type->isFunctionProtoType()},
        //{"isFunc", type->isFunctionType()},
        //{"isFunctionNoProtoType", type->isFunctionNoProtoType()},

        // Template stuff
        {"isTemplated", type->isTemplateTypeParmType()},
        //{"templateArgs", type->getAs},

        {"isEnum", type->isEnumeralType()},
        {"isStruct", type->isStructureType()},
        {"isClass", type->isClassType()},
        {"isUnion", type->isUnionType()},

        {"typeClass", type->getTypeClassName()},
        //{"inSTD", type->getAsTagDecl()->isInStdNamespace()}
        //{"isFunc", type->isFunctionPointerType()},
        //{"funcParams", GetFuncParams(type->getAsTagDecl()->getAsFunction())},
    };
}

json GetSubRecords(const CXXRecordDecl *record) {
  json subRecords = json::array();

  return subRecords;
}

json GetFuncParams(FunctionDecl *func) {
    json params = json::array();
    for (auto param : func->parameters()) {
        params.push_back(
            merge(GetTypeInfo(param->getType(), param->getNameAsString()),
                {
                    //{"isFunc", param->isFunctionOrFunctionTemplate()},
                    //{"funcInfo", GetFuncInfo(param->getAsFunction())},
                }));
    }
    return params;
}

json GetFuncInfo(NamedDecl *decl) {
    auto func = decl->getAsFunction();
    return {
        {"name", func->getNameAsString()},
        {"returns", GetTypeInfo(func->getReturnType())},
        {"params", GetFuncParams(func)},
        {"isOverloadedOperator", func->isOverloadedOperator()},
    };
}

bool isSingleton = false;
json GetRecordMethods(const CXXRecordDecl *record) {
    json methods = json::array();
    isSingleton = false;
    // Iterate over record's methods
    for (auto method : record->methods()) {
        // Templates are not fun
        if (method->isTemplated() ||
            method->getVisibility() != clang::Visibility::DefaultVisibility)
        continue;

        // Set method info from record
        methods.push_back(
            merge(GetFuncInfo(method),
                {
                    {"isCtor", method->getKind() == Decl::CXXConstructor},
                    {"isDtor", method->getKind() == Decl::CXXDestructor},
                    {"isStatic", method->isStatic()},
                    {"isVirtual", method->isVirtual()},
                }));

        if (method->isStatic() && method->getNameAsString() == "Instance")
        isSingleton = true;
    }

    return methods;
}

json GetEnumValues(EnumDecl* enumDecl)
{
    json values = json::array();
    for(auto decl : enumDecl->decls())
    {
        if(decl->getKind() != Decl::EnumConstant) continue;

        auto ec = static_cast<EnumConstantDecl*>(decl);
        values.push_back({
            {"name", ec->getNameAsString()},
            {"value", ec->getInitVal().getExtValue()},
        });
    }

    return values;
}

json GetRecordDecls(const CXXRecordDecl *record)
{
    json decls = json::array();
    for(auto decl : record->decls())
    {
        switch (decl->getKind())
        {
            case Decl::Enum:
            {
                auto ed = static_cast<EnumDecl*>(decl);
                decls.push_back({
                    {"kind", decl->getDeclKindName()},
                    {"name", ed->getNameAsString()},
                    {"qualName", ed->getQualifiedNameAsString()},
                    {"values", GetEnumValues(ed)},
                });
                break;
            }
        }
    }

    return decls;
}

// CXXRecordDecl are structs/classes/unions
DeclarationMatcher recordMatcher = cxxRecordDecl().bind("records");
json classes = json::array();

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

        // Only handle local records, no templates
        if (!IsRecordLocal(rd) || rd->isTemplated() ||
            !rd->isThisDeclarationADefinition())
        return;

        // debug?
        rd->dump(ast);
        astOut << ast.str();
        // debug?

        // Set the JSON info
        classes.push_back({
            {"name", rd->getNameAsString()},

            //{"subrecords", GetSubRecords(rd)}, // couldn't figure it out
            {"methods", GetRecordMethods(rd)},
            {"decls", GetRecordDecls(rd)},
            //{"parent", rd->getParent()}

            {"isEnum", rd->isEnum()},
            {"isStruct", rd->isStruct()},
            {"isClass", rd->isClass()},
            {"isUnion", rd->isUnion()},
            {"isSingleton", isSingleton},

            {"location", rd->getLocation().printToString(
                            rd->getASTContext().getSourceManager())},
        });
    }
};

int main(int argc, const char **argv) {
    pp.SuppressTagKeyword = true;

    CommonOptionsParser optionsParser(argc, argv, category, 0);

    ClangTool tool(optionsParser.getCompilations(),
                    optionsParser.getSourcePathList());

    RecordHandler recordHandler;
    MatchFinder finder;
    finder.addMatcher(recordMatcher, &recordHandler);

    std::cout << "Processing API..." << std::endl;
    tool.run(newFrontendActionFactory(&finder).get());

    std::ofstream out(OUTPUT_NAME);
    out << std::setw(4) << classes;
    std::cout << "Output to " << OUTPUT_NAME << std::endl;
    return 0;
}
