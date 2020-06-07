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
    result = std::regex_replace(result, reg::star, "Ptr");
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

    // differences between windows and unix
// #ifndef _WIN32
//     result = std::regex_replace(result, reg::stdcoloncolon, "");
// #endif

    result = std::regex_replace(result, reg::coloncolon_, "_");
    result = std::regex_replace(result, reg::classstructenum, "");
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
    static Typedata GetSizedPadding(clang::QualType type, const clang::ASTContext& context)
    {
        logd("// Placing as sized padding instead");
        Typedata data;
        data.kind = Typedata::ARRAY;
        data.forwardDecl = "";
        data.ctype = "char";
        data.array_size = context.getTypeSizeInCharsIfKnown(type).getValueOr(CharUnits::fromQuantity(0)).getQuantity();
        if(data.array_size == 0)
        {
            data.ok = false;
            logd("// Could not retrieve proper type size");
        }

        return data;
    };

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
        json data = {
            {"name", ctype},
            {"kind", kind},
            {"kind_str", kind_str()},
        };

        if(pointee)
        {
            data["pointee"] = pointee->json_data();
        }

        if(kind == FUNCTION_POINTER)
        {
            data["returns"] = fn_return->json_data();
            for(auto& p : fn_params)
                data["params"].push_back(p.json_data());
        }
        else if(kind == ARRAY)
        {
            data["array_size"] = array_size;
        }

        return data;
    }

    bool ok = true;
    std::string ctype;
    std::string forwardDecl;

    // pointer
    Typedata* pointee = nullptr;

    // function pointer
    Typedata* fn_return = nullptr;
    std::vector<Typedata> fn_params;

    // array
    size_t array_size = 0;
    
    Typedata() {};
    Typedata(clang::QualType type, const clang::ASTContext& context)
    {
        type = 
            type->getCanonicalTypeInternal()
            // .getUnqualifiedType()
            // .getDesugaredType(context)
            ;

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
        if(type->isFunctionPointerType())
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

                fn_params.push_back(paramtypedata);
            }

            if(badparams)
            {
                logd("// bad params");
                ok = false;
                return;
            }

            auto rettype = fn->getReturnType();
            fn_return = new Typedata(rettype, context);
            if(!fn_return->ok)
            {
                ok = false;
                return;
            }
            
            ctype = fn_return->ctype+("(*)(")+cfnparams+(")");
        }
        else if(type->isFundamentalType() || type->isBooleanType())
        {
            logd("// fundamental");
            kind = FUNDAMENTAL;
#ifndef _WIN32
            {
                auto count = reg::count_matches(cpptypestr, reg::long_);
                if(count <= 1 && count > 0)
                {
                    cpptypestr = std::regex_replace(cpptypestr, reg::long_, "long long");
                }
            }
#endif
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
            // logd("// NOT IMPLEMENTED");
            // auto arraytype = context.getAsDependentSizedArrayType(type);

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
            logd("// Could not determine ctype for " + cpptypestr);

            ok = false;
            return;
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
            pointee = new Typedata(data);
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

    std::string GetCTypeWithName(const std::string& name)
    {
        if(kind == ARRAY)
        {
            if(array_size > 0)
            {
                // ok
                return forwardDecl+ctype+" "+name+"["+std::to_string(array_size)+"]";
            }
            else {
                logd("// ARRAY SIZE WAS 0, outputting as simple pointer");
                return forwardDecl+ctype+"* "+name;
            }
        }

        // just return normally
        return forwardDecl+ctype+" "+name;
    }
};
