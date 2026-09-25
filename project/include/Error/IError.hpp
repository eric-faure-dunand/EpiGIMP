
#ifndef IERROR_HPP
    #define IERROR_HPP
    #include <exception>
    #include <iostream>

    #include "printer.hpp"

class IError : public std::exception {
public:
    virtual int code() const noexcept = 0;
    virtual const char *what() const noexcept = 0;
};

inline std::ostream& operator<<(std::ostream& input, const IError& e) {
    if (e.code() == 84)
        input << raytracer::Color::RED << "Error : " << raytracer::Color::RESET;
    else if (e.code() == 2)
        input << raytracer::Color::MAGENTA << "File not Found : " << raytracer::Color::RESET;
    else
        input << raytracer::Color::YELLOW << "Warning : " << raytracer::Color::RESET;
    return input << e.what() << std::endl;
};

#endif
