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
    result = std::regex_replace(result, reg::coloncolon, "_");

    result = std::regex_replace(result, reg::classstructenum, "");
    result = std::regex_replace(result, reg::ampersand, "Ref");
    result = std::regex_replace(result, reg::spacestar, "Ptr");
    result = std::regex_replace(result, reg::commaspace, "_");
    result = std::regex_replace(result, reg::notstartwithalt_, "");
    // result = std::regex_replace(result, reg::templates, "_");
    result = std::regex_replace(result, reg::less, "_");
    result = std::regex_replace(result, reg::more, "");
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
    result = std::regex_replace(result, reg::commaspace, "_");
    result = std::regex_replace(result, reg::notstartwithalt_, "");
    // result = std::regex_replace(result, reg::templates, "_");
    result = std::regex_replace(result, reg::less, "_");
    result = std::regex_replace(result, reg::more, "");
    result = std::regex_replace(result, reg::space, "");

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

inline bool SymbolDefined(const std::string& symbolname)
{
    if(capisymbols.find(symbolname) != capisymbols.end())
        return true;
    else return false;
}

extern void HandleEnum(const EnumDecl* enumdecl);

// Controls and generated type definitions if needed
inline void HandleType(clang::QualType type)
{
    // Right now only forward-declared enums aren't allowed
    // (msvc has extensions that make it work)
    // we only need to pre-define enums before usage

    if(type->isEnumeralType())
    {
        HandleEnum(dyn_cast<EnumDecl>(type->getAsTagDecl()));
    }
}

struct Typedata
{
    enum Kind
    {
        FUNDAMENTAL,
        ENUMERAL,
        REFERENCE,
        POINTER,
        ARRAY,
        FUNCTION_POINTER,
        STRUCT
    } kind;

    std::string kind_str()
    {
        switch (kind)
        {
        case FUNDAMENTAL:
            return "fundamental";
        case ENUMERAL:
            return "enumeral";
        case REFERENCE:
            return "reference";
        case POINTER:
            return "pointer";
        case ARRAY:
            return "array";
        case FUNCTION_POINTER:
            return "function_pointer";
        case STRUCT:
            return "struct";
        }
    }

    json json_data()
    {
        return {
            {"name", ctype},
            {"kind", kind},
            {"kind_str", kind_str()}
        };
    }

    bool ok = true;
    Typedata* pointee = nullptr;
    std::string ctype;
    std::string forwardDecl;
    // size_t elements = 0;

    Typedata(clang::QualType type, const clang::ASTContext& context)
    {
        type = type.getCanonicalType()
            .getUnqualifiedType()
            .getDesugaredType(context);

        HandleType(type);

        auto cpptypestr = type.getAsString();
        logd("// cpptype " << cpptypestr);

        // if(type->isLValueReferenceType())
        // {
        //     logd("// IS LVALUE REFERENCE");
        // }

        if(cpptypestr.find("std::") != std::string::npos)
        {
            logd("// type is in STD");
            ok = false;
            return;
        }
        // TEMP
        // else if (cpptypestr.find("(&)[") != std::string::npos) {
        //     logd("// type is reference to sized array or something, cant do these ones yet chief");
        //     ok = false;
        //     return;
        // }
        // TEMP
        else if(type->isFunctionPointerType())
        {
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
            kind = FUNDAMENTAL;
            ctype = cpptypestr;
        }
        else if(type->isEnumeralType())
        {
            logd("// enumaral type");
            kind = ENUMERAL;
            // forward declared enums not allowed
            // forwardDecl = "enum ";
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
            ok = pointee->ok;
            ctype = pointee->ctype+"*";

            // Number of elements
            // auto vectype = type->getAsArrayTypeUnsafe()
            //     ->getAs<clang::VectorType>();
            // elements = vectype->getNumElements();
            // logd("// number of elements: " << std::to_string(elements));

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
            if(!pointee->ok)
            {
                logd("// pointee was not ok");
                return;
            }
            else if (pointee->kind == ARRAY)
            {
                logd("// array reference");
                ctype = pointee->ctype;
            }
            else {
                ctype = pointee->ctype+"*";
            }

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
            if(!pointee->ok)
            {
                logd("// pointee was not ok");
                return;
            }

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

        case FUNDAMENTAL:
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
