#pragma once

#include <exception>

class Ex : public std::exception {
    const char* _msg;
    Ex();
public:
    Ex(const char* msg) noexcept : _msg(msg) {};
    Ex(const std::string &msg) noexcept : _msg(msg.c_str()) {};

    const char* what() const noexcept {
        return _msg;
    }
};