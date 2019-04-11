#pragma once

#include <string>

#include "nlohmann/json.hpp"

namespace capi {

class Function;
class Info
{
public:
    static std::initializer_list<std::string> ignoredRecords;
    static std::initializer_list<std::string> ignoredPaths;

    void Write(Function* func)
    {

    }
};

}
