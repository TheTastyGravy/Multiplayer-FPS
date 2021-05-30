# Multiplayer game
A simple multiplayer FPS made in C++ with RakNet, Raylib with a cpp wrapper, and a custom Networked Physics System.

The game consists of two executables: the server and the client. Multiple clients need to connect to a server in order to play. By default, both will use local host, meaning they will only work on a single device, but this can be changed in `main.cpp` in each project to connect to any IP address.

To run the game, first launch the server console app and check that it has set up correctly, then launch a client app on the same device as the server. The player controlls are WASD to move, space to jump, and left mouse button to fire a rocket.
