#pragma once
#include <string>
#include <vector>
#include "../helpers/stringutils.h"

// Base class
class TextCommand {
protected: 
    std::vector<std::string> _args;

public:

    virtual void run(GameState &state) = 0;

    void set_args(std::string& args) {
        _args = split_str(args);
    }

    void set_args(const std::vector<std::string>& args) {
        _args.clear();
        copy(args.begin(), args.end(), back_inserter(_args));
    }
    
    virtual std::string get_name() const = 0;
};

class DrawCommand : public TextCommand {
public:
     std::string get_name() const {
        return "Draw";
     }

     void run(GameState& state) {

     }
};

class PlayCommand : public TextCommand {
public:
    std::string get_name() const {
        return "Play";
    }

    void run(GameState& state) {

    }
};

class UnoCommand : public TextCommand {
public:
    std::string get_name() const {
        return "Uno";
    }

    void run(GameState& state) {

    }
};