#include "tool.hpp"

void HandleEnum(const EnumDecl* enumdecl)
{
    // Data
    json enumjson;
    auto enumname = ToCIdentifier(enumdecl->getQualifiedNameAsString());
    //logfile << "Enum: " << enumname << std::endl;

    if(SymbolDefined(enumname))
    {
        logd("// Symbol is already defined");
        return;
    }

    // enums
    std::stringstream enumcontent;
    for(auto e = enumdecl->enumerator_begin(); e != enumdecl->enumerator_end(); )
    {
        // data
        auto ename = ToUpper(enumname)+("_")+e->getName().str();
        auto eval = e->getInitVal().getLimitedValue();
        //logfile << ename << " = " << eval << std::endl;

        // header
        std::string s = ("    ") + ename + (" = ") + std::to_string(eval);
        s += (++e != enumdecl->enumerator_end() ? ",\n" : "\n");
        enumcontent << s;

        // json
        enumjson["enumerators"].push_back({
            {"name", ename},
            {"value", eval},
        });
    }

    // Write to header
    capixheader("typedef ", "");
    capiheader("enum " << enumname << " {\n" 
        << enumcontent.str() 
        << "};\n" << std::endl);


    // json
    capijson["enums"][enumname] = enumjson;

    capisymbols.emplace(enumname);
}

static auto enumMatcher = enumDecl(

    unless(isExpansionInSystemHeader())
).bind("enum");

static Handler enumHandler(enumMatcher, [](const MatchFinder::MatchResult& result){
    auto enumdecl = result.Nodes.getNodeAs<EnumDecl>("enum");

    HandleEnum(enumdecl);
    
    // ast
    //enumdecl->dump(ast);
});
