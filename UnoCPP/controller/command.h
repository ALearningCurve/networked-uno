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

/// <summary>
/// Interface for text commands that operate on the uno game state
/// </summary>
class UnoGameTextCommand {
public:
    virtual ~UnoGameTextCommand() = default;
    /// <summary>
    /// Runs this command on the specified views and for the given player in the given game
    /// </summary>
    /// <param name="state">Game state to run the command on</param>
    /// <param name="userView">view that outputs the result to the requester</param>
    /// <param name="wholeGameView">view that outputs the result to all members of the lobby</param>
    /// <param name="player">player to run the command for</param>
    virtual void run(GameState &state, TextView* userView, TextView* wholeGameView, Player* player) = 0;

    /// <summary>
    /// Gets the string representation of this command
    /// </summary>
    /// <returns>String representation</returns>
    virtual std::string get_name() const = 0;

    /// <summary>
    /// Returns whether or not this command takes a whole turn to executre. If true, it does.
    /// </summary>
    /// <returns>If true, signifies this command takes a whole turn to execute</returns>
    virtual bool takes_whole_turn() const = 0;
};

/// <summary>
/// Uno Game command that writes help information to the view when run
/// </summary>
class HelpCommand : public UnoGameTextCommand {
public:
    HelpCommand() {}
    // see UnoGameTextCommand for function comment 
    std::string get_name() const {
        return "help";
    }
    // see UnoGameTextCommand for function comment. Class comment describes what this method does
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

    // see UnoGameTextCommand for function comment 
    bool takes_whole_turn() const {
        return false;
    }
};

/// <summary>
/// Uno Game Command that when run shows the game state of the requester to the requester's view
/// </summary>
class ShowGameInfoCommand : public UnoGameTextCommand {
public:
    ShowGameInfoCommand() {}
    // see UnoGameTextCommand for function comment 
    std::string get_name() const {
        return "show game info";
    }
    // see UnoGameTextCommand for function comment. Class comment describes what this method does.
    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        userView->info(userView->stringify_game_start_for_player(state, player));
    }
    // see UnoGameTextCommand for function comment 
    bool takes_whole_turn() const {
        return false;
    }
};

/// <summary>
/// Uno Game Command that when run draws a card for the requester.
/// </summary>
class DrawCommand : public UnoGameTextCommand {
public:
    DrawCommand() {}
    // see UnoGameTextCommand for function comment 
     std::string get_name() const {
        return "Draw";
     }
     // see UnoGameTextCommand for function comment. Class comment describes what this method does.
     void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
         std::shared_ptr<Card> card = state.draw_for_player(player);
         userView->info("Drew a " + userView->stringify_card(*card));
         wholeGameView->info(player->get_name() + " just drew a card");
     }
     // see UnoGameTextCommand for function comment 
     bool takes_whole_turn() const {
         return true;
     }
};

/// <summary>
/// Uno Game Command that plays the requested card for the user. 
/// </summary>
class PlayCommand : public UnoGameTextCommand {
    const int _card_num;
    const std::optional<std::string> _wild_color_choice;
public:
    /// <summary>
    /// Makes command useing args in vector of strings. This is the expected
    /// types/values for each index:
    /// [0]: int: index of card in hand to play
    /// [1]: string?: OPTIONAL if card is wild, the color of the card should be specified here
    /// </summary>
    /// <param name="args">string vector with the commands to process</param>
    PlayCommand(const std::vector<std::string>& args): _card_num(VecValidator::get_int(args, 0)), _wild_color_choice(VecValidator::get_string_optional(args, 1)) {}
    // see UnoGameTextCommand for function comment 
    std::string get_name() const {
        return "Play";
    }
    // see UnoGameTextCommand for function comment. Class/constructor comment has more information.
    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        auto canPlay = state.can_play(player, _card_num, _wild_color_choice);
        if (canPlay != std::nullopt) {
            throw std::invalid_argument("Cannot play that card: " + *canPlay);
        }
        const auto& card = state.play_for_player(player, _card_num, _wild_color_choice);
        wholeGameView->info(player->get_name() + " just played a " + wholeGameView->stringify_card(card));
    }

    bool takes_whole_turn() const {
        return true;
    }
};

/// <summary>
/// Uno Game Command that represents the action of the requester yelling UNO!
/// </summary>
class UnoCommand : public UnoGameTextCommand {
public:
    // see UnoGameTextCommand for function comment 
    std::string get_name() const {
        return "Uno";
    }
    // see UnoGameTextCommand for function comment. Class comment has more information
    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        state.player_said_uno(player);
        wholeGameView->alert(player->get_name() + " successfully called \"uno\"! They are now uno immune and/or some players got caught not saying uno!");
    }
    // see UnoGameTextCommand for function comment 
    bool takes_whole_turn() const {
        return false;
    }
};

/// <summary>
/// Uno Game Command that represents the action of playing a card and then immediatly saying uno
/// </summary>
class PlayThenUnoCommand : public UnoGameTextCommand {
    PlayCommand _play;
    UnoCommand _uno;
public:
    /// <summary>
    /// Vector should be in same format as the vector provided for the PlayCommand
    /// </summary>
    /// <param name="args">vector of parameters to configure this command</param>
    PlayThenUnoCommand(const std::vector<std::string>& args) : _play(args), _uno() {}
    // see UnoGameTextCommand for function comment 
    std::string get_name() const {
        return "Play then Uno";
    }
    // see UnoGameTextCommand for function comment. See class/constructor comment for more info
    void run(GameState& state, TextView* userView, TextView* wholeGameView, Player* player) {
        // make sure this command can be run (player must have only 1 card)
        if (state.get_current_player()->get_hand().get_number_cards() != 2) {
            throw std::invalid_argument("Can only use this command when you have 2 cards in your hand!");
        }
        _play.run(state, userView, wholeGameView, player);
        _uno.run(state, userView, wholeGameView, player);
    }
    // see UnoGameTextCommand for function comment 
    bool takes_whole_turn() const {
        return true;
    }
};

/// <summary>
/// Interface for text comamnds that modify a lobby on networked players using SOCKETS.
/// </summary>
class LobbyTextCommand {
public:
    virtual ~LobbyTextCommand() = default;
    /// <summary>
    /// Runs this command to modify the LobbyManager state using the 
    /// requester SOCKET as the user who is running this command.
    /// </summary>
    /// <param name="requester">the user who we should run this command for</param>
    /// <param name="state">the lobby manager instance to modify</param>
    /// <param name="requesterView">view that only outputs data to the requester</param>
    /// <param name="allInLobbyView">view that outputs data to a whole lobby for the current requester</param>
    virtual void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) = 0;
    
    /// <summary>
    /// Returns the string representation of the command
    /// </summary>
    /// <returns>string representation of the command</returns>
    virtual std::string get_name() const = 0;
};

/// <summary>
/// Lobby Command that outputs to the requester view the help menu.
/// </summary>
class LobbyHelpCommand : public LobbyTextCommand {
public:
    LobbyHelpCommand(const std::vector<std::string>& args) {}
    // see LobbyTextCommand for function comment 
    std::string get_name() const {
        return "lobby help";
    }
    // see LobbyTextCommand for function comment. Class comment has more information
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

/// <summary>
/// Lobby Text Command that joins a user to a lobby.
/// </summary>
class JoinLobbyCommand : public LobbyTextCommand {
    std::string _lobby_id;
    std::string _requester_name;
public:
    /// <summary>
    /// Makes command using args in vector of strings. This is the expected
    /// types/values for each index:
    /// [0]: string: lobby id to join
    /// [1]: string: requester username (must be between 1 and 15 characters)
    /// </summary>
    /// <param name="args">string vector with the commands to process</param>
    JoinLobbyCommand(const std::vector<std::string>& args) : _lobby_id(VecValidator::get_string(args, 0)), _requester_name(VecValidator::get_string(args, 1)) {
        if (_requester_name.size() < 1 || _requester_name.size() > 15) {
            throw std::exception("Player name must be between 1 and 15 characters");
        }
    }
    // see LobbyTextCommand for function comment. Class comment contains more information.
    std::string get_name() const {
        return "lobby help";
    }
    // see LobbyTextCommand for function comment. 
    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        SocketPlayer p{ requester, std::make_unique<Player>(_requester_name) };
        state.add_player_to_lobby(_lobby_id, std::move(p));
        requesterView->info("Successfully joined lobby");
        allInLobbyView->info("Player \"" + _requester_name + "\" joined the lobby");
        Lobby& l = state.get_lobby(_lobby_id);
        if (l._started) {
            allInLobbyView->alert("Game has now started! Its " + l._game->get_current_player()->get_name() + "'s turn to start! Type \"gameinfo\" to see your hand now!");
        }
    }
};

/// <summary>
/// Makes command using args in vector of strings. This is the expected
/// types/values for each index:
/// [0]: string: lobby id to join
/// [1]: string: requester username (must be between 1 and 15 characters)
/// [2]: int:    number of playrers that should be in the lobby
/// </summary>
/// <param name="args">string vector with the commands to process</param>
class NewLobbyCommand : public LobbyTextCommand {
    std::string _lobby_id;
    std::string _requester_name;
    int numPlayers;
    JoinLobbyCommand joinCommand;
public:
    NewLobbyCommand(const std::vector<std::string>& args) : 
        _lobby_id(VecValidator::get_string(args, 0)), 
        _requester_name(VecValidator::get_string(args, 1)), 
        numPlayers(VecValidator::get_int(args, 2)),
        joinCommand(args)
    {}
    // see LobbyTextCommand for function comment 
    std::string get_name() const {
        return "new lobby";
    }
    // see LobbyTextCommand for function comment. Class/constructor comments 
    // have more information about what happens when this command is run
    void run(SOCKET requester, LobbyManager& state, TextView* requesterView, TextView* allInLobbyView) {
        state.create_lobby(_lobby_id, requester, numPlayers);
        requesterView->info("Created lobby \"" + _lobby_id + "\"");
        joinCommand.run(requester, state, requesterView, allInLobbyView);
    }
};



