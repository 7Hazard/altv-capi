#pragma once

#include "regex.hpp"

#include <string>
#include <regex>
#include <algorithm>
#include <map>
#include <set>

static std::string ToLower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static std::string ToUpper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

static std::string GetCName(const std::string& str)
{
    std::string result = str;
    
    //result = std::regex_replace(result, reg::underscore, "_$&");
    result = std::regex_replace(result, std::regex("::_|::"), "");

    //result = ToLower(result);

    return result;
}

static std::string ToCIdentifier(const std::string& str, const std::string& suffix = "")
{
    std::string result = str;

    //result = std::regex_replace(result, reg::underscore, "_$&");
    result = std::regex_replace(result, reg::coloncolon_, "_");

    result = std::regex_replace(result, reg::classstructenum, "");
    result = std::regex_replace(result, reg::ampersand, "Ref");
    result = std::regex_replace(result, reg::spacestar, "Ptr");
    result = std::regex_replace(result, reg::commaspace, "");
    result = std::regex_replace(result, reg::notstartwithalt_, "");
    result = std::regex_replace(result, reg::templates, "");
    result = std::regex_replace(result, reg::space, "");

    //result+=suffix;
    // Replace duplicates for suffixes
    //if(suffix!="")
    //{
    //    auto tofind = suffix+suffix;
    //    auto f = result.find(tofind);
    //    while(f != std::string::npos)
    //    {
    //        result.replace(f, 2, suffix);
    //        f = result.find(tofind, f);
    //    }
    //}
//
    //// Replace double underscores
    //{
    //    auto f = result.find("__");
    //    while(f != std::string::npos)
    //    {
    //        result.replace(f, 2, "_");
    //        f = result.find("__", f);
    //    }
    //}

    //result = ToLower(result);

    return result;
}

static std::string ToCType(const std::string& str, const std::string& suffix = "")
{
    std::string result = str;

    result = std::regex_replace(result, reg::coloncolon_, "_");
    result = std::regex_replace(result, reg::sugarquals, "");
    result = std::regex_replace(result, reg::ampersand, "*");
    result = std::regex_replace(result, reg::commaspace, "");
    result = std::regex_replace(result, reg::notstartwithalt_, "");
    result = std::regex_replace(result, reg::templates, "");

    return result;
}

/*
static std::string ToCType(const std::string& str, const std::string& suffix = "")
{
    std::string result = str;

    result = std::regex_replace(result, reg::underscore, "_$&");
    result = std::regex_replace(result, reg::coloncolon_, "_");

    result = std::regex_replace(result, reg::classstruct, "");
    //result = std::regex_replace(result, reg::spacestar, "_ptr");
    result = std::regex_replace(result, reg::commaspace, "_");
    result = std::regex_replace(result, reg::notstartwithalt_, "");
    result = std::regex_replace(result, reg::templates, "_");
    //result = std::regex_replace(result, reg::space, "_");

    result+=suffix;
    // Replace duplicates for suffixes
    if(suffix!="")
    {
        auto tofind = suffix+suffix;
        auto f = result.find(tofind);
        while(f != std::string::npos)
        {
            result.replace(f, 2, suffix);
            f = result.find(tofind, f);
        }
    }

    // Replace double underscores
    {
        auto f = result.find("__");
        while(f != std::string::npos)
        {
            result.replace(f, 2, "_");
            f = result.find("__", f);
        }
    }

    result = ToLower(result);

    return result;
}
*/

/*
inline std::map<const CXXRecordDecl*, std::string>& records()
{
    static std::map<const CXXRecordDecl*, std::string> r;
    return r;
}

static std::string GetCType(const QualType& type)
{
    std::string result;

    auto utype = type.getUnqualifiedType();

    auto record = type->getAsCXXRecordDecl();
    if(record)
    {
        //std::string ProcessRecord(const CXXRecordDecl* record);
        //auto f = records.find(record);
        //if(f != records.end()) result = f->second;
        //else result = ProcessRecord(record);

        // add pointers and references
        //result = 
        //return result;
    }

    if(utype->isTemplateTypeParmType() && utype->isPointerType())
    {
        return "void *";
    }

    result = GetCName(utype.getAsString());
    result = std::regex_replace(result, std::regex("class"), "struct");

    return result;
}

static std::string GetSizedType(size_t bytes)
{
    return "SIZED_TYPE("+std::to_string(bytes)+")";
}
*/

static std::string GetCFuncPtr(const QualType& type, const std::string& name)
{
    auto fn = type->getPointeeType()->getAs<FunctionProtoType>();
                
    std::string cfnparams, cfnparam;
    bool badparams = false;
    for(auto fnparam : fn->param_types())
    {
        if(fnparam.getAsString().find("std::") != std::string::npos)
        {
            logd("// Param type is in STD\n");
            badparams = true;
            continue;
        }
        else if(fnparam->isFunctionPointerType())
        {
            cfnparam = GetCFuncPtr(fnparam, "");
            if(cfnparam.empty())
            {
                logd("// Bad funcptr\n");
                badparams = true;
                continue;
            }
        }
        else if(fnparam->isFundamentalType() || fnparam->isBooleanType()
            || (fnparam->isPointerType() && fnparam->getPointeeType()->isFundamentalType())
            )
        {
            cfnparam = fnparam.getAsString();
        }
        else if(fnparam->isEnumeralType())
        {
            cfnparam = ToCType(fnparam.getAsString());
        }
        else {
            cfnparam = ("struct ")+ToCType(fnparam.getAsString());
        }
        
        if(!cfnparams.empty())
            cfnparams+=", ";
        cfnparams += cfnparam;
    }
    if(badparams)
        return "";
    else
        return ToCType(fn->getReturnType().getAsString())
            +(" (*")+name+(")(")+cfnparams+(")");
}
