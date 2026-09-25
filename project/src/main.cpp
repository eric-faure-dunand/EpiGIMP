#include "main.hpp"

static bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return 84;
    }

    if (!fileExists(argv[1])) {
        FileNotFound err(argv[1]);
        std::cerr << err;
        return err.code();
    }

    try {
        gimp::Core core(argv[1]);
        core.run();
    } catch (const IError& e) {
        std::cerr << e;
        return e.code();
    }

    return 0;
}