# Multiplayer game
A simple multiplayer FPS made in C++ with RakNet, Raylib with a cpp wrapper, and a custom Networked Physics System.

The game consists of two executables: the server and the client. Multiple clients need to connect to a server in order to play. By default, both will use local host, meaning thewy will only work on a single device, but this can be changed in `main.cpp` in each project to connect to any IP address.