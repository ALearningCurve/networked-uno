#pragma once
#include <string>
#include <vector>
#include "../helpers/stringutils.h"
#include "../LobbyManager.h"

class VecValidator {
public:
    static const std::string& getAndEnsureIndex(const std::vector<std::string>& vec, const int& index) {
        if (vec.size() <= index) {
            throw std::invalid_argument("missing argument #" + std::to_string(index + 1));
        }
        return vec.at(index);
    }
    static int getInt(const std::vector<std::string>& vec, const int& index) {
        const std::string& str = VecValidator::getAndEnsureIndex(vec, index);
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
        return VecValidator::getAndEnsureIndex(vec, index);
    }

    static const std::optional<std::string> getStringOptional(const std::vector<std::string>& vec, const int& index) {
        if (vec.size() > index) {
            return getAndEnsureIndex(vec, index);
        }
        return {};
    }
};

// Base class for commands that modify the game state
class UnoGameTextCommand {
public:
    virtual ~UnoGameTextCommand() = default;
    virtual void run(GameState &state, TextView* view) = 0;
    virtual std::string get_name() const = 0;
    virtual bool takes_whole_turn() const = 0;
};

class HelpCommand : public UnoGameTextCommand {
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

class DrawCommand : public UnoGameTextCommand {
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

class PlayCommand : public UnoGameTextCommand {
    const int _cardNum;
    const std::optional<std::string> _wildColorChoice;
public:
    PlayCommand(const std::vector<std::string>& args): _cardNum(VecValidator::getInt(args, 0)), _wildColorChoice(VecValidator::getStringOptional(args, 1)) {}

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

class UnoCommand : public UnoGameTextCommand {
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

class PlayThenUnoCommand : public UnoGameTextCommand {
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

class LobbyTextCommand {
public:
    virtual ~LobbyTextCommand() = default;
    virtual void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) = 0;
    virtual std::string get_name() const = 0;
};

//m["new"] = [](auto args) { return std::make_shared<NewLobbyCommand>(); };
//m["join"] = [](auto args) { return std::make_shared<JoinLobbyCommand>(); };

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
            "                   - name: (between 1 and 15 characters (no whitespace)) string of your player's name in the lobby. \n"
        );
    }
};

class JoinLobbyCommand : public LobbyTextCommand {
    std::string lobbyId;
    std::string creatorName;
public:
    JoinLobbyCommand(const std::vector<std::string>& args) : lobbyId(VecValidator::getString(args, 0)), creatorName(VecValidator::getString(args, 1)) {
        if (creatorName.size() < 1 || creatorName.size() > 15) {
            throw std::exception("Player name must be between 1 and 15 characters");
        }
    }

    std::string get_name() const {
        return "lobby help";
    }

    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        std::unique_ptr player = std::make_unique<SocketPlayer>(creatorName, requester);
        state.addPlayerToLobby(lobbyId, std::move(player));
        requesterView->info("Successfully joined lobby");
        allInLobbyView->info("Player \"" + creatorName + "\" joined the lobby");
        Lobby& l = state.getLobby(lobbyId);
        if (l._started) {
            allInLobbyView->alert("Game has now started! Its " + l._game->get_current_player()->get_name() + "'s turn to start!");
        }
    }

    bool takes_whole_turn() const {
        return false;
    }
};

class NewLobbyCommand : public LobbyTextCommand {
    std::string lobbyId;
    std::string creatorName;
    int numPlayers;
    JoinLobbyCommand joinCommand;
public:
    NewLobbyCommand(const std::vector<std::string>& args) : 
        lobbyId(VecValidator::getString(args, 0)), 
        creatorName(VecValidator::getString(args, 1)), 
        numPlayers(VecValidator::getInt(args, 2)),
        joinCommand(args)
    {}

    std::string get_name() const {
        return "new lobby";
    }

    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        state.createLobby(lobbyId, requester, numPlayers);
        requesterView->info("Created lobby \"" + lobbyId + "\"");
        joinCommand.run(requester, state, requesterView, allInLobbyView);
    }
};



