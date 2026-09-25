#ifndef CORE_HPP
    #define CORE_HPP
    #include <memory>

    #include "UI.hpp"

namespace gimp {

class Core {
    std::unique_ptr<UI> Ui;

public:
    Core(const std::string& imagePath);
    ~Core() = default;

    void run();
};

}

#endif
