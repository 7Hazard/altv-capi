#pragma once

#include <initializer_list>
#include <algorithm>
#include <string>
#include <functional>
#include <regex>

std::string ToCFormat(std::string str)
{
    auto result = str;

    std::regex reg("::(C(?=[A-Z])|I(?=[A-Z]))|::");
    while(std::regex_search(result, reg))
    {
        result = std::regex_replace(result, reg, "_");
    }

    for(size_t i = 0; i < result.size(); i++)
    {
        if(islower(result[i]) && isupper(result[i+1]))
        {
            result.insert(i+1, "_");
        }
    }

    // lowercase
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

// Checks if the record is local to the processed files
bool IsRecordPathValid(const CXXRecordDecl *record) {
    FullSourceLoc fullSourceLoc(
        record->getLocation(),
        record->getASTContext().getSourceManager()
    );
    auto name = fs::canonical(
        fullSourceLoc.getFileLoc().printToString(record->getASTContext().getSourceManager())
        ).string();
    std::replace(name.begin(), name.end(), '\\', '/'); // from \\ to / if on windows
    
    if(fullSourceLoc.isValid() && !fullSourceLoc.isInSystemHeader())
    {
        for (auto str : ignoredPaths)
        {
            if(name.find(str) != std::string::npos)
                return false;
        }

        return true;
    }
    
    return false;
}

bool IsRecordInvalid(const CXXRecordDecl* rd)
{
    auto name = rd->getQualifiedNameAsString();
    for (auto v : ignoredClasses)
    {
        if (name == v) return true;
    }

    return !rd->isThisDeclarationADefinition() 
        || rd->isInStdNamespace()
        || !IsRecordPathValid(rd)
        ;
}

// Public methods, no ctors and dtors
bool IsMethodInvalid(CXXMethodDecl* method)
{
    auto methodreturns = method->getReturnType();
    // no std returns
    if(methodreturns->isStructureOrClassType() && methodreturns->getAsCXXRecordDecl()->isInStdNamespace())
        return true;

    return method->getVisibility() != clang::Visibility::DefaultVisibility
        || method->getKind() == Decl::CXXConstructor
        || method->getKind() == Decl::CXXDestructor
        || method->isOverloadedOperator()
        || method->isTemplated()
        ;
}

json merge(const json &a, const json &b) {
    json result = a;
    json tmp = b;
    for (json::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        result[it.key()] = it.value();
    }
    return result;
}

std::string GetAccessStr(AccessSpecifier as)
{
    switch(as)
    {
        case AS_private: return "private";
        case AS_protected: return "protected";
        case AS_public: return "public";
        default: return "";
    }
}

std::string GetVisibilityStr(Visibility v)
{
    switch(v)
    {
        case HiddenVisibility: return "hidden";
        case ProtectedVisibility: return "protected";
        case DefaultVisibility: return "default";
        default: return "";
    }
}
