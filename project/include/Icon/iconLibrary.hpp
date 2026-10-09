#ifndef ICON_LIBRARY_HPP
    #define ICON_LIBRARY_HPP

    #include <memory>
    #include <string>
    #include <unordered_map>

    #include "icon.hpp"

namespace gimp {

class IconLibrary {
    std::string BaseDir;
    std::unordered_map<std::string, std::unique_ptr<Icon>> Icons;

public:
    IconLibrary(const std::string& baseDir);
    ~IconLibrary() = default;

    bool Load(const std::string& name, const std::string& file, bool monochrome = true);
    const Icon* Get(const std::string& name) const;
};

}

#endif
