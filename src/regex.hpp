#pragma once

#include <regex>

namespace reg
{
    static std::regex fnptrname("\\(\\*\\)");
    static std::regex classstructenum("class |struct |enum ");
    static std::regex sugarquals("class |enum |struct |const ");
    static std::regex commaspace(", ");
    static std::regex notstartwithalt_("(?!^)alt_");
    static std::regex templates("(<|>| >)+");
    static std::regex space(" ");
    static std::regex ampersand("&"); 
    static std::regex spaceampersand(" &");
    static std::regex spacestar(" \\*");
    static std::regex alt_t("(alt_.*(?= ))|(alt_.*)");
    static std::regex __more("__+");
    static std::regex trailing_("_+$");
    static std::regex underscore("(?!^[A-Z])(?:[A-Z]*)(C[A-Z]|I[A-Z]|[A-Z])");
    static std::regex cstyle("(?!^[A-Z])(?:[A-Z]*)(C[A-Z]|I[A-Z]|[A-Z])");
    static std::regex coloncolon_("::_|::");
    static std::regex alt_("alt_.*\\w");

}
