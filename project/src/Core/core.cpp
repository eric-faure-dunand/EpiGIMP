#include "core.hpp"

namespace gimp {

Core::Core(const std::string& imagePath) {
    Ui = std::make_unique<UI>(imagePath);
}

void Core::run() {
    while (Ui->open()) {
        Ui->DrawFrame();
    }
}

}
