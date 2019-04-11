#pragma once

#include <map>

#include "nlohmann/json.hpp"

namespace capi {

struct Type
{
    static std::map<std::string, Type*> cache;

public:

    std::string kind;
    bool ok = true;
    std::string name;
    nlohmann::json json;

    Type* Get(clang::QualType type)
    {

    }

    Type* Get(std::string name)
    {
        try
        {
            return cache.at(name);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

protected:
    void Cache()
    {
        cache.emplace(name, this);
    }
};

}
