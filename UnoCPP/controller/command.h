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
        const std::string& str = TextCommand::getAndEnsureIndex(vec, index);
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

    static const std::optional<std::string> getStringOptional(const std::vector<std::string>& vec, const int& index) {
        if (vec.size() > index) {
            return getAndEnsureIndex(vec, index);
        }
        return {};
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
         std::shared_ptr<Card> card = state.draw_for_player(state.get_current_player());
         view.output("Drew a " + card->format());
     }
};

class PlayCommand : public TextCommand {
    const int _cardNum;
    const std::optional<std::string> _wildColorChoice;
public:
    PlayCommand(int cardNum) : _cardNum(cardNum) {}
    PlayCommand(const std::vector<std::string>& args): _cardNum(TextCommand::getInt(args, 0)), _wildColorChoice(TextCommand::getStringOptional(args, 1)) {}

    std::string get_name() const {
        return "Play";
    }

    void run(GameState& state, TextView& view) {
        auto canPlay = state.can_play(state.get_current_player(), _cardNum, _wildColorChoice);
        if (canPlay != std::nullopt) {
            throw std::invalid_argument("Cannot play that card: " + *canPlay);
        }

        state.play_for_player(state.get_current_player(), _cardNum, _wildColorChoice);
    }
};

class UnoCommand : public TextCommand {
public:
    std::string get_name() const {
        return "Uno";
    }

    void run(GameState& state, TextView& view) {
        state.player_said_uno(state.get_current_player());
    }
};