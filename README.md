# The Boom Box

Teesside University - Multiplayer and Social Games.

This is a multiplayer game project  and multiplatform (Windows and MacOS compatibility) that uses TCP and UDP connections to work. The game consists on a catch game. One of the players will start with a bomb and will have to catch another player before the time runs out or he will be disqualified, and the last man standing wins the game.

The game is composed by various scenes:
- Log In: The player has to log in to the server using his own username and password and the ip of the server.
- Waiting room: The players can see the information of the other players connected and also chat with them.
- Game scene: Where a maximum of 4 players can join and play the catch game.

The game is using SFML (for the graphics rendering and networking), Box2D (for the collision and physics) and SFGUI (for the basic GUI of the game)

In order to build the source code you can use the following depending on your OS:
* Windows (64-bit): On the build folder there is some lua files that will generate a Visual Studio 2015 project file configured and ready to go, just needs you to use the generate_vs2015.bat file and will do the magic.

* Mac OSX: For the Mac users, there is an already created XCode and Xamarin project on the project folder, but it needs some libraries dependencies installed on your system (SFML and Box2D), so you can simply execute the InstallMacDependences on the build/OSX folder.

Game Instructions:

When you have all the project built and working, you will see that there is a client and a server, you should start the server first in order to allow the clients to connect to it.

At the beginning of the server execution, it will print the IP and the Port of the server, this will be asked at the beginning of the client execution. 

The server is made to allow a maximum of 4 players at the same time.

The game consists on a catch game. One of the players will start with a bomb and will have to catch another player before the time runs out or he will be disqualified, and the last man standing wins the game.

You can control the player by pressing the arrow keys to move and the space key to jump. 
