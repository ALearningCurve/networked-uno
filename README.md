# Multi-Modal C++ TCP Socket Uno Server
## Demo Videos
Click on the thumbnails to watch a demo video for each of the modes!
### Networked Mode
[![LOCAL MODE VIDEO](https://img.youtube.com/vi/0m9v5iwZ0AU/0.jpg)](https://www.youtube.com/watch?v=0m9v5iwZ0AU&list=PLiNsRg8Y2Zs21uPbsQfBDqgWwx87OYVkA&index=1)

### Local Mode
[![LOCAL MODE VIDEO](https://img.youtube.com/vi/XiHzQRQydKs/0.jpg)](https://www.youtube.com/watch?v=XiHzQRQydKs&list=PLiNsRg8Y2Zs21uPbsQfBDqgWwx87OYVkA&index=2)
## About
### Why?
I created this project with the the goal to expand my C++ knowledge, get a better understanding of complex applications using C++, and explore what low level programming and networking looks like. To that extent, in this project I created a non-blocking TCP Server using the WinSock2 API that runs a multi-client uno game server which supports multiple clients, lobbies, and games at the same time. The C++ Server can also be run in "local" mode to forgo networking for simpler experience where all players play from the same terminal.

This was a great learning experience because I made the decision to only use functionality from the standard C++ library and windows APIs. This means I really got a chance to learn how to solve problems with a C++ mindset instead of blindly using someone else's implementation. Additionally, it gave me the chance to explore what low-level networking looks like, something that is often hidden by most networking libraries.

### Design
#### The C++ Server
The C++ server is obviously written in C++, so I made an effort to use classes and interfaces where appropriate to make the design extendable. As such, I tried separate the code following MVC practices as best as possible. Additionally, I wanted the C++ Uno game to be playable both using a networked game and one where all the clients can play on one computer locally. This requires two different paradigms to support the different playing modes, but a lot of it was abstracted using these aforementioned interfaces. One example of this how the `./UnoCPP/controller/command.h` defines interface types for commands that let us apply the command design pattern. This easily lets us handle user input in a concise, extendable, and simple way. Likewise, using polymorphism with the `TextView` interface lets us reuse the majority of the code for handling user input and displaying results between both the socket based networked controller and the non-networked local controller with ease.

From a high level, the general model for the server is based around lobbies which link the SOCKET of a user to each game instance:
- the TCPServer keeps track of active connections via sockets and provides an event which our controller can handle (onConnect, onDisconnect, onInput)
- Via these events, the controller can map each socket to a lobby/active game via the LobbyManager
- Each lobby contains other players in the game (both their SOCKETs and Player instance used in the game), which lets the controller determine who is who
- Each lobby has at most one uno game (instance of GameState), which is the game that is being played in the lobby
- Any communication to the player (in either game mode) is done through an instance of TextView
- Users can send commands to the server via text that is used to change their lobby state or to take an action in the game depending on if they are in a lobby.

### The Python Client
"I thought this was a C++ project, so why is there a python client?" you might ask. Well, since I used the WinSock2 API in the CPP code, only windows users can run the C++ project. So, just for simplicity (since the main learning focus is server code for me) and portability across operating systems, I decided to create a quick and simple python script for a client to connect to the server.

## Running this project
Because this project uses the WinSock2 API, unfortunately the C++ Server can only be run on a windows machine. However, since the Client is written in Python, a user can connect to the C++ server from any machine! 

The next steps assume you have downloaded the release zip, extracted it, and have your working directory where you extracted the zip. You can find a release under the [releases GitHub page](https://github.com/ALearningCurve/Uno/releases). 

### Networked Mode
In this mode, running the C++ executable `UnoCPP.exe` with the argument `server`  (so `./UnoCPP.exe server`) will start the server on port `1337`.

You can then connect to this server by running `python Client.py`. However, by default it will look for the server on `localhost`. To change the host, change the `HOST` variable within the `Client.py` script manually.

### Local Player
In this mode, running the C++ executable `UnoCPP.exe` with argument `local` (so `./UnoCPP.exe local`) will start a game of UNO in which you can take turns playing by typing into the command prompt when it is your turn.

## Building
I built this project using Visual Studio and C++ 17, so you *should* find similar success with that setup. Note that because the project uses WinSock2 API, you probably will only be able to build the project on a windows machine.

## Improvements
This project was made over the course of many months and served as a learning project for me. As a result of the learning that took place throughout the project I recognize some improvements that can be made, but for the sake of me pursuing my other passions, I am not sure I will come back to fix them:
- For clarity/efficiency it would be nice to replace shared_ptr with unique_ptr to reflect ownership of the Card class in the Player, GameState, and Deck classes (right now they use shared_ptr)
- Create interface types for the TCPServer and models to enable them to easily be swapped out in the future (for good design). Also add a translation layer so that the C++ server can run on other platforms and is not limited by WinSock2.
- Add tests and make sure our classes are testable
- Add custom exception implementations to allow the controllers to distinguish between user input errors and actual internal errors
