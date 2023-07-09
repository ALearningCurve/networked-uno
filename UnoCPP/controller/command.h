#pragma once
#include <string>
#include <vector>
#include "../helpers/stringutils.h"
#include "../models/lobby_manager.h"

// our command classes use a vector of strings to parse user input. Thus, this namespace holds some utiliy functions to 
// get strings from the vector and potentially convert the type of the string.
class VecValidator {
public:
    static const std::string& get_and_ensure_index(const std::vector<std::string>& vec, const int& index) {
        if (vec.size() <= index) {
            throw std::invalid_argument("missing argument #" + std::to_string(index + 1));
        }
        return vec.at(index);
    }
    static int get_int(const std::vector<std::string>& vec, const int& index) {
        const std::string& str = VecValidator::get_and_ensure_index(vec, index);
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

    static const std::string& get_string(const std::vector<std::string>& vec, const int& index) {
        return VecValidator::get_and_ensure_index(vec, index);
    }

    static const std::optional<std::string> get_string_optional(const std::vector<std::string>& vec, const int& index) {
        if (vec.size() > index) {
            return get_and_ensure_index(vec, index);
        }
        return {};
    }
};

// Base class for commands that modify the game state
class UnoGameTextCommand {
public:
    virtual ~UnoGameTextCommand() = default;
    virtual void run(GameState &state, TextView* userView, TextView* wholeGameView, Player* player) = 0;
    virtual std::string get_name() const = 0;
    virtual bool takes_whole_turn() const = 0;
};

class HelpCommand : public UnoGameTextCommand {
public:
    HelpCommand() {}

    std::string get_name() const {
        return "help";
    }

    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        userView->info(
            "Help Menu: \n"
            "  h:           show help menu\n"
            "  gameinfo:    show game information such as your hand, the last played card, and other player's cards"
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

class ShowGameInfoCommand : public UnoGameTextCommand {
public:
    ShowGameInfoCommand() {}

    std::string get_name() const {
        return "show game info";
    }

    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        userView->info(userView->stringify_game_start_for_player(state, player));
    }

    bool takes_whole_turn() const {
        return false;
    }
};

class DrawCommand : public UnoGameTextCommand {
public:
    DrawCommand() {}
    
     std::string get_name() const {
        return "Draw";
     }

     void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
         std::shared_ptr<Card> card = state.draw_for_player(player);
         userView->info("Drew a " + userView->stringify_card(*card));
         wholeGameView->info(player->get_name() + " just drew a card");
     }

     bool takes_whole_turn() const {
         return true;
     }
};

class PlayCommand : public UnoGameTextCommand {
    const int _cardNum;
    const std::optional<std::string> _wildColorChoice;
public:
    PlayCommand(const std::vector<std::string>& args): _cardNum(VecValidator::get_int(args, 0)), _wildColorChoice(VecValidator::get_string_optional(args, 1)) {}

    std::string get_name() const {
        return "Play";
    }

    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        auto canPlay = state.can_play(player, _cardNum, _wildColorChoice);
        if (canPlay != std::nullopt) {
            throw std::invalid_argument("Cannot play that card: " + *canPlay);
        }
        const auto& card = state.play_for_player(player, _cardNum, _wildColorChoice);
        wholeGameView->info(player->get_name() + " just played a " + wholeGameView->stringify_card(card));
    }

    bool takes_whole_turn() const {
        return true;
    }
};

class UnoCommand : public UnoGameTextCommand {
public:
    std::string get_name() const {
        return "Uno";
    }

    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        try {
            state.player_said_uno(player);
        }
        catch (std::exception_ptr ex) {
            std::rethrow_exception(ex);
        }
        wholeGameView->alert(player->get_name() + " successfully called \"uno\"! They are now uno immune and/or some players got caught not saying uno!");
    }

    bool takes_whole_turn() const {
        return false;
    }
};

class PlayThenUnoCommand : public UnoGameTextCommand {
    PlayCommand _play;
    UnoCommand _uno;
public:
    PlayThenUnoCommand(const std::vector<std::string>& args) : _play(args), _uno() {}

    std::string get_name() const {
        return "Play then Uno";
    }

    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        // make sure this command can be run (player must have only 1 card)
        if (state.get_current_player()->get_hand().get_number_cards() != 2) {
            throw std::invalid_argument("Can only use this command when you have 2 cards in your hand!");
        }
        _play.run(state, userView, wholeGameView, player);
        _uno.run(state, userView, wholeGameView, player);
    }

    bool takes_whole_turn() const {
        return true;
    }
};

class LobbyTextCommand {
public:
    virtual ~LobbyTextCommand() = default;
    virtual void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) = 0;
    virtual std::string get_name() const = 0;
};

class LobbyHelpCommand : public LobbyTextCommand {
public:
    LobbyHelpCommand(const std::vector<std::string>& args) {}

    std::string get_name() const {
        return "lobby help";
    }

    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        requesterView->info(
            "Help Menu: \n"
            "  help:        show help menu\n"
            "  new:         create a new lobby and joins you into that lobby. Takes the following arguments:\n"
            "                   - lobbyId: string (between 1 and 15 characters (no whitepace))\n"   
            "                   - name: (between 1 and 15 characters (no whitespace)) string of your player's name in the lobby. \n"          
            "                   - players: number (betwewn 1 and 4). Automatically starts game once this number of players join the lobby\n" 
            "  join:        joins a lobby. Takes following arguments: \n"
            "                   - lobbyId: string of the lobby to join (between 1 and 15 charactes)\n" 
            "                   - name: string (between 1 and 15 characters (no whitespace)) of your player's name in the lobby. \n"
        );
    }
};

class JoinLobbyCommand : public LobbyTextCommand {
    std::string lobbyId;
    std::string creatorName;
public:
    JoinLobbyCommand(const std::vector<std::string>& args) : lobbyId(VecValidator::get_string(args, 0)), creatorName(VecValidator::get_string(args, 1)) {
        if (creatorName.size() < 1 || creatorName.size() > 15) {
            throw std::exception("Player name must be between 1 and 15 characters");
        }
    }

    std::string get_name() const {
        return "lobby help";
    }

    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        SocketPlayer p{ requester, std::make_unique<Player>(creatorName) };
        state.add_player_to_lobby(lobbyId, std::move(p));
        requesterView->info("Successfully joined lobby");
        allInLobbyView->info("Player \"" + creatorName + "\" joined the lobby");
        Lobby& l = state.get_lobby(lobbyId);
        if (l._started) {
            allInLobbyView->alert("Game has now started! Its " + l._game->get_current_player()->get_name() + "'s turn to start! Type \"gameinfo\" to see your hand now!");
        }
    }
};

class NewLobbyCommand : public LobbyTextCommand {
    std::string lobbyId;
    std::string creatorName;
    int numPlayers;
    JoinLobbyCommand joinCommand;
public:
    NewLobbyCommand(const std::vector<std::string>& args) : 
        lobbyId(VecValidator::get_string(args, 0)), 
        creatorName(VecValidator::get_string(args, 1)), 
        numPlayers(VecValidator::get_int(args, 2)),
        joinCommand(args)
    {}

    std::string get_name() const {
        return "new lobby";
    }

    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        state.create_lobby(lobbyId, requester, numPlayers);
        requesterView->info("Created lobby \"" + lobbyId + "\"");
        joinCommand.run(requester, state, requesterView, allInLobbyView);
    }
};



