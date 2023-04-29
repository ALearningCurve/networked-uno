#pragma once
#include <string>
#include <vector>
#include "../helpers/stringutils.h"

// Base class
class TextCommand {
protected: 
    static const std::string& getAndEnsureIndex(const std::vector<std::string>& vec, const int& index) {
        if (vec.size() <= index) {
            throw std::invalid_argument("missing argument #" + std::to_string(index + 1));
        }
        return vec.at(index);
    }
    static int getInt(const std::vector<std::string> &vec, const int &index) {
        const std::string &str = TextCommand::getAndEnsureIndex(vec, index);
        try {
           return std::stoi(str);
        }
        catch (const std::invalid_argument&) {
            throw std::invalid_argument("given \"" + str + "\" when an integer was expected");
        } 
        catch (const std::out_of_range&) {
            throw std::invalid_argument("given an input that is too large to parse into an integer");
        }
    }

    static const std::string& getString(const std::vector<std::string>& vec, const int& index) {
        return TextCommand::getAndEnsureIndex(vec, index);
    }

    static bool getBool(std::string& str) {
        return false;
    }

public:
    virtual ~TextCommand() = default;
    virtual void run(GameState &state, TextView &view) = 0;
    virtual std::string get_name() const = 0;
};

class HelpCommand : public TextCommand {
public:
    HelpCommand() {}

    std::string get_name() const {
        return "help";
    }

    void run(GameState& state, TextView& view) {
        view.output(
            "Help Menu: \n"
            "  h: help \n"
            "  draw: draw a card\n"
            "  play: play a card\n"
            "  uno: shout uno\n"
            "  q: quit"
        );
    }
};

class DrawCommand : public TextCommand {
public:
    DrawCommand() {}
    
     std::string get_name() const {
        return "Draw";
     }

     void run(GameState& state, TextView& view) {

     }
};

class PlayCommand : public TextCommand {
    const int _cardNum;
public:
    PlayCommand(int cardNum) : _cardNum(cardNum) {}
    PlayCommand(const std::vector<std::string>& args): _cardNum(TextCommand::getInt(args, 0)) {}

    std::string get_name() const {
        return "Play";
    }

    void run(GameState& state, TextView& view) {
        
    }
};

class UnoCommand : public TextCommand {
public:
    std::string get_name() const {
        return "Uno";
    }

    void run(GameState& state, TextView& view) {

    }
};