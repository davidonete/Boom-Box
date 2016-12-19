#include <iostream>

#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "Assets.h"
#include "GameManager.h"

#define SCALE 30 //pass from pixels to box2D positions

/*
void Init()
{
    GameManager::GetInstance()->Init();

    GameManager::GetInstance()->SpawnObject(Vec2(200.0f, 20.0f), Vec2(32.0f, 32.0f), DynamicBody, 1.0f, 0.7f, "box.png", Vec2(16.0f, 16.0f));
    //GameManager::GetInstance()->SpawnObject(Vec2(200.0f, 20.0f), Vec2(32.0f, 32.0f), DynamicBody, 1.0f, 0.7f, "box.png", Vec2(16.0f, 16.0f));
    GameManager::GetInstance()->SpawnObject(Vec2(400.0f, 500.0f), Vec2(800.0f, 16.0f), StaticBody, 0.0f, 5.0f, "ground.png", Vec2(400.0f, 8.0f));
}

void Remove()
{
    //To Do: Remove GameManager
}

void Input(GameManager* GM)
{
    GM->Input();
}

void Update(GameManager* GM)
{
    sf::Event event;
    while (GM->GetWindow()->pollEvent(event))
    {
        // Close window: exit
        if (event.type == sf::Event::Closed)
        GM->GetWindow()->close();
    }

    GM->Update();
}

void Render(GameManager* GM)
{
    GM->Render();
}

int main()
{
    GameManager* GM = GameManager::GetInstance();

    Init();
    while (GM->GetWindow()->isOpen())
    {
        Input(GM);
        Update(GM);
        Render(GM);
    }
    return EXIT_SUCCESS;
}
*/

//http://www.iforce2d.net/b2dtut/constant-speed

//-------------------------------------------------------------------------------
#define TCP_CONNECTION

struct Packet
{
	unsigned int id;
	float x, y;
};

const unsigned short PORT = 8080;
const std::string IPADDRESS("127.0.0.1");

std::string msgSend;

sf::TcpSocket tcpSocket;
sf::UdpSocket udpSocket;

sf::Mutex globalMutex;
bool quit = false;
bool connected = false;

void ClientConnection()
{
    if (!connected)
    {
        #ifdef TCP_CONNECTION
        tcpSocket.disconnect();
        if (tcpSocket.connect(IPADDRESS, PORT) == sf::Socket::Done)
        {
            std::cout << "\nConnected";
            connected = true;
        }
        else
            connected = false;

        #else
        //UDP does not need a previous connection for sending
        connected = true;
		udpSocket.setBlocking(false);
		//Bind to a local port to receive from server
        udpSocket.bind(sf::Socket::AnyPort);
        #endif
    }
}

void GetInput(void)
{
    if (connected)
    {
        std::string s;
        std::cout << "\nEnter \"exit\" to quit or message to send: ";
        std::cin >> s;

        if (s == "exit")
        {
            quit = true;
            return;
        }

        // Create bytes to send
        int BufferLenght = s.length() + 1;
        char *Buffer = new char[BufferLenght];
        std::strcpy(Buffer, s.c_str());

        #ifdef TCP_CONNECTION

		Packet packet;
		packet.id = 1;
		packet.x = 128.5f;
		packet.y = 0.5f;

		//Sending Side
		char buffer[sizeof(packet)];
		memcpy(buffer, &packet, sizeof(packet));

		//Receiving Side
		Packet tmp;
		memcpy(&tmp, buffer, sizeof(tmp));
		std::cout << tmp.x;

        //if (tcpSocket.send(Buffer, sizeof(char) * BufferLenght) != sf::Socket::Done)
        if(tcpSocket.send(buffer, sizeof(packet)) != sf::Socket::Done)
		{
            std::cout << "error sending...";
            connected = false;
        }

        Buffer = new char[128];
        std::size_t Received;

        //it is blocking until it receives data (can set the socket to non-blocking)
        if (tcpSocket.receive(Buffer, sizeof(char) * BufferLenght, Received) != sf::Socket::Done)
        {
            std::cout << "error receiving...";
            connected = false;
        }
        else
            std::cout << "Received " << Received << " bytes from server: " << Buffer;

        delete Buffer;

        #else
        if (udpSocket.send(Buffer, sizeof(char) * BufferLenght, IPADDRESS, PORT) != sf::Socket::Done)
            std::cout << "error sending...";

        Buffer = new char[128];
        std::size_t Received;

        sf::IpAddress Sender;
        unsigned short Port;

        if (udpSocket.receive(Buffer, sizeof(char) * BufferLenght, Received, Sender, Port) != sf::Socket::Done)
            std::cout << "error receiving... ";
        else
            std::cout << "Received " << Received << " bytes from server: " << Buffer << " " << Sender << ":" << Port;

        #endif
    }
    else
        ClientConnection();
}

int main() 
{
    //sf::Thread* thread = 0;

    //thread = new sf::Thread(&DoStuff);
    //thread->launch();

    ClientConnection();

    while (!quit)
        GetInput();

    #ifdef TCP_CONNECTION
    tcpSocket.disconnect();

    #else
    //udpSocket.close();
    udpSocket.unbind();

    #endif

    return 0;
}

//http://www.sfml-dev.org/tutorials/1.6/network-sockets.php