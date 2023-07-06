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
    virtual void run(GameState &state, TextView* view) = 0;
    virtual std::string get_name() const = 0;
    virtual bool takes_whole_turn() const = 0;
};

class HelpCommand : public TextCommand {
public:
    HelpCommand() {}

    std::string get_name() const {
        return "help";
    }

    void run(GameState& state, TextView* view) {
        view->info(
            "Help Menu: \n"
            "  h:           show help menu\n"
            "  draw:        draw a card from the deck\n"
            "  play:        play a card. Takes following arguments: \n"
            "                   - index: numeric index of card in your hand (starting from 0)\n" 
            "                   - color: if card being played is wild, must specify color of red, green, blue, or yellow)\n"
            "  play+uno:    plays a card and then calls uno. do this if after you play a card you would\n"
            "               only have 1 card to make yourself immune to unos! Takes same arguments as \"play\" command\n"
            "  uno:         shout uno! Note, to say uno on the turn you played a card, use the \"play+uno\" command\n"
            "  q:           quits the game"
        ); 
    }

    bool takes_whole_turn() const {
        return false;
    }
};

class DrawCommand : public TextCommand {
public:
    DrawCommand() {}
    
     std::string get_name() const {
        return "Draw";
     }

     void run(GameState& state, TextView* view) {
         std::shared_ptr<Card> card = state.draw_for_player(state.get_current_player());
         view->info("Drew a " + view->stringify_card(*card));
     }

     bool takes_whole_turn() const {
         return true;
     }
};

class PlayCommand : public TextCommand {
    const int _cardNum;
    const std::optional<std::string> _wildColorChoice;
public:
    PlayCommand(const std::vector<std::string>& args): _cardNum(TextCommand::getInt(args, 0)), _wildColorChoice(TextCommand::getStringOptional(args, 1)) {}

    std::string get_name() const {
        return "Play";
    }

    void run(GameState& state, TextView* view) {
        auto canPlay = state.can_play(state.get_current_player(), _cardNum, _wildColorChoice);
        if (canPlay != std::nullopt) {
            throw std::invalid_argument("Cannot play that card: " + *canPlay);
        }

        state.play_for_player(state.get_current_player(), _cardNum, _wildColorChoice);
    }

    bool takes_whole_turn() const {
        return true;
    }
};

class UnoCommand : public TextCommand {
public:
    std::string get_name() const {
        return "Uno";
    }

    void run(GameState& state, TextView* view) {
        state.player_said_uno(state.get_current_player());
    }

    bool takes_whole_turn() const {
        return false;
    }
};

class PlayThenUnoCommand : public TextCommand {
    PlayCommand _play;
    UnoCommand _uno;
public:
    PlayThenUnoCommand(const std::vector<std::string>& args) : _play(args), _uno() {}

    std::string get_name() const {
        return "Play then Uno";
    }

    void run(GameState& state, TextView* view) {
        // make sure this command can be run (player must have only 1 card)
        if (state.get_current_player()->get_hand().get_number_cards() != 2) {
            throw std::invalid_argument("Can only use this command when you have 2 cards in your hand!");
        }
        _play.run(state, view);
        _uno.run(state, view);
    }

    bool takes_whole_turn() const {
        return true;
    }
};