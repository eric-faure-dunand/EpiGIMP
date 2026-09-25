#include "core.hpp"

namespace gimp {

Core::Core() {
    Ui = std::make_unique<UI>();
}

void Core::run() {
    while (Ui->open()) {
        Ui->DrawFrame();
    }
}

}
