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

//-------------------------------------------------------------------------------

const unsigned short PORT = 8080;
const std::string IPADDRESS("127.0.0.1");

std::string msgSend;

sf::TcpSocket socket;
sf::Mutex globalMutex;
bool quit = false;
bool connected = false;

void DoStuff(void)
{
    if (connected)
    {
        static std::string oldMsg;
        //while (!quit)
        //{
            /*
            std::cout << "Do Stuff\n";
            sf::Packet packetSend;
            globalMutex.lock();
            packetSend << msgSend;
            globalMutex.unlock();

            socket.send(packetSend);
            */

            std::string msg;
            sf::Packet packetReceive;

            socket.receive(packetReceive);
            if (packetReceive >> msg)
            {
                if (oldMsg != msg)
                    if (!msg.empty())
                    {
                        std::cout << msg << std::endl;
                        oldMsg = msg;
                    }
            }
        //}
    }
}

void Client(void)
{
    if (!connected)
    {
        if (socket.connect(IPADDRESS, PORT) == sf::Socket::Done)
        {
            std::cout << "Connected\n";
            connected = true;
        }
        else
            connected = false;
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
            quit = true;

        sf::Packet packetSend;
        packetSend << msgSend;

        sf::UdpSocket Socket;

        // Create bytes to send
        int BufferLenght = s.length() + 1;
        char *Buffer = new char[BufferLenght];
        std::strcpy(Buffer, s.c_str());

        if (socket.send(Buffer, sizeof(char) * BufferLenght) != sf::Socket::Done)
            std::cout << "error sending...";

        delete Buffer;
        //socket.send(packetSend);

        /*
        globalMutex.lock();
        msgSend = s;
        globalMutex.unlock();
        */
    }
}

int main() 
{
    //sf::Thread* thread = 0;

    //thread = new sf::Thread(&DoStuff);
    //thread->launch();

    while (!quit)
    {
        Client();
        GetInput();
        DoStuff();
    }

    /*
    if (thread)
    {
       
        thread->wait();
        delete thread;
    }
    */
    socket.disconnect();

    return 0;
}

//http://www.iforce2d.net/b2dtut/constant-speed