#include "iconLibrary.hpp"

#include "Warning.hpp"

namespace gimp {

IconLibrary::IconLibrary(const std::string& baseDir) : BaseDir(baseDir) {}

bool IconLibrary::Load(const std::string& name, const std::string& file, bool monochrome) {
    try {
        Icons[name] = std::make_unique<Icon>(BaseDir + "/" + file, monochrome);
    } catch (const IError& e) {
        std::cerr << Warning("Icone introuvable : " + std::string(e.what()));
        return false;
    }
    return true;
}

const Icon* IconLibrary::Get(const std::string& name) const {
    auto it = Icons.find(name);
    if (it == Icons.end())
        return nullptr;
    return it->second.get();
}

}
