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
    result = std::regex_replace(result, reg::spaceampersand, "*");
    result = std::regex_replace(result, reg::spacestar, "*");
    //result = std::regex_replace(result, reg::star, "*");
    result = std::regex_replace(result, reg::commaspace, "");
    result = std::regex_replace(result, reg::notstartwithalt_, "");
    result = std::regex_replace(result, reg::templates, "");
    result = std::regex_replace(result, reg::space, "_");

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

static std::string GetDateAndTime()
{
    auto time = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(time);
    return std::ctime(&end_time);
}


struct Typedata
{
    enum Kind
    {
        FUNDEMENTAL,
        ENUMERAL,
        REFERENCE,
        POINTER,
        ARRAY,
        FUNCTION_POINTER,
        STRUCT
    } kind;

    bool ok = true;
    Typedata* pointee = nullptr;
    std::string ctype;
    std::string forwardDecl;

    Typedata(clang::QualType type, const clang::ASTContext& context)
    {
        type = type.getCanonicalType()
            .getUnqualifiedType()
            .getDesugaredType(context);

        auto cpptypestr = type.getAsString();
        logd("// cpptype " << cpptypestr);

        if(cpptypestr.find("std::") != std::string::npos)
        {
            logd("// type is in STD\n");
            ok = false;
            return;
        }
        else if(type->isFunctionPointerType())
        {
            // todo
            // abort();

            logd("// function pointer");
            kind = FUNCTION_POINTER;

            auto fn = type->getPointeeType()->getAs<FunctionProtoType>();
                
            std::string cfnparams, cfnparam;
            bool badparams = false;
            for(auto paramtype : fn->param_types())
            {
                auto paramtypedata = Typedata(paramtype, context);
                if(paramtypedata.ok)
                    badparams = true;

                if(!cfnparams.empty())
                    cfnparams+=", ";
                cfnparams += paramtypedata.ctype;
            }

            if(badparams)
            {
                logd("// bad params");
                ok = false;
                return;
            }

            auto rettype = fn->getReturnType();
            auto rettypedata = Typedata(rettype, context);
            if(!rettypedata.ok)
            {
                ok = false;
                return;
            }
            
            ctype = rettypedata.ctype+("(*)(")+cfnparams+(")");
        }
        else if(type->isFundamentalType() || type->isBooleanType())
        {
            logd("// fundamental");
            kind = FUNDEMENTAL;
            ctype = cpptypestr;
        }
        else if(type->isEnumeralType())
        {
            logd("// enumaral type");
            kind = ENUMERAL;
            forwardDecl = "enum ";
            ctype = ToCType(cpptypestr);
        }
        else if(type->isArrayType())
        {
            logd("// array type");
            kind = ARRAY;
            pointee = new Typedata(
                type->getPointeeOrArrayElementType()
                    ->getCanonicalTypeUnqualified()
                    .operator clang::QualType(),
                context
            );
            ctype = pointee->ctype+"*";

            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl; 
        }
        else if(type->isReferenceType())
        {
            logd("// Reference type");
            kind = REFERENCE;
            pointee = new Typedata(type->getPointeeType(), context);
            ctype = pointee->ctype+"*";

            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl;
        }
        else if(type->isPointerType())
        {
            logd("// Pointer type");
            kind = POINTER;
            pointee = new Typedata(type->getPointeeType(), context);

            ctype = pointee->ctype+"*";

            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl;
        }
        else if(type->isStructureOrClassType())
        {
            kind = STRUCT;
            forwardDecl = "struct ";
            ctype = ToCType(cpptypestr);
        }
        else
        {
            logd("// Could not process type");
            ok = false;
            return;
            // abort();
        }
    }

    // Manual typing
    // FUNDEMENTAL kind will use _ctype param for fundemental type
    Typedata(Typedata& data, Typedata::Kind kind, std::string _ctype = "void")
        : kind(kind)
    {
        switch (kind)
        {
        case POINTER:
        {
            pointee = &data;
            ctype = pointee->ctype+"*";
            
            // get the bottom pointee's forwardDecl
            Typedata* bottomPointee = pointee;
            while(bottomPointee->pointee)
                bottomPointee = bottomPointee->pointee;
            forwardDecl = bottomPointee->forwardDecl;
        } break;

        case FUNDEMENTAL:
        {
            ctype = _ctype;
            forwardDecl = "";
            pointee = nullptr;
        } break;
        
        default:
        {
            abort();
        } break;
        }
    }
};
