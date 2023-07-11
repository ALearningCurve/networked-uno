#pragma once

#include <exception>

// TODO use this exception and make some other custom exceptions to 
// be able to differentatiate between internal errors and the ones
// caused by user input when running commands

/// <summary>
/// Custom error exception 
/// </summary>
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