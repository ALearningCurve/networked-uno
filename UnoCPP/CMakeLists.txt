# CMakeList.txt : CMake project for UnoCPP, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("UnoCPP")

# Add source to this project's executable.
add_executable (UnoCPP "UnoCPP.cpp" 
"UnoCPP.h" 
"models/player.h" 
"models/card.h"
"models/deck.h"
"controller/controller.h" 
"view/Header1.h" 
"models/card.cpp"
"models/player.cpp" 
"models/hand.h"
"models/hand.cpp" 
"models/deck.cpp"
"models/game_state.h"
"models/game_state.cpp")

# TODO: Add tests and install targets if needed.
