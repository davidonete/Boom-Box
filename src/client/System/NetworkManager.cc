#include "System/NetworkManager.h"

#include <iostream>

const unsigned short PORT = 8080;
const std::string IPADDRESS("127.0.0.1");

NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {}

void NetworkManager::Init()
{
    //tcpSocket.setBlocking(false);
    client.authority = false;
    client.id = 0;
}

bool NetworkManager::Connect(ConnectionType type)
{
    if (type == TCP)
    {
        if (tcpSocket.connect(IPADDRESS, PORT) == sf::Socket::Done)
        {
            std::cout << "Connected.\n";
            return true;
        }
        else
            return false;
    }
    else
    {
        //UDP does not need a previous connection for sending
        udpSocket.setBlocking(false);
        //Bind to a local port to receive from server
        udpSocket.bind(sf::Socket::AnyPort);
        return true;
    }
}

void NetworkManager::Disconnect(ConnectionType type)
{
    if (type == TCP)
    {
        tcpSocket.send("", 1);
        tcpSocket.disconnect();
    }
    else
        udpSocket.unbind();
}

bool NetworkManager::SendPacket(ConnectionType type, GamePacket packet)
{
    //TO DO
    if (type == TCP)
    {
        std::cout << "Error sending TCP packet...";
        return false;
    }
    else
    {
        std::cout << "Error sending UDP packet...";
        return false;
    }
}

bool NetworkManager::SendPacket(LogInPacket packet)
{
    // Create bytes to send
    const int size = sizeof(packet);
    char buffer[size];
    memcpy(buffer, &packet, size);

    if (tcpSocket.send(buffer, size) != sf::Socket::Done)
    {
        std::cout << "Error sending TCP packet...";
        return false;
    }

    return true;
}

bool NetworkManager::SendPacket(ChatPacket packet)
{
    // Create bytes to send
    const int size = sizeof(packet);
    char buffer[size];
    memcpy(buffer, &packet, size);

    if (tcpSocket.send(buffer, size) != sf::Socket::Done)
    {
        std::cout << "Error sending TCP packet...";
        return false;
    }

    return true;
}

bool NetworkManager::ReceivePacket(ServerConfirmPacket &packet)
{
    //Prepare buffer to receive
    const int size = sizeof(ServerConfirmPacket);
    char buffer[size];
    std::size_t Received;

    //it is blocking until it receives data (can set the socket to non-blocking)
    if (tcpSocket.receive(buffer, size, Received) != sf::Socket::Done)
    {
        std::cout << "Error receiving TCP packet...";
        return false;
    }

    memcpy(&packet, buffer, size);
    return true;
}

bool NetworkManager::ReceivePacket(ChatPacket &packet)
{
    //Prepare buffer to receive
    const int size = sizeof(ChatPacket);
    char buffer[size];
    std::size_t Received;

    //it is blocking until it receives data (can set the socket to non-blocking)
    if (tcpSocket.receive(buffer, size, Received) != sf::Socket::Done)
    {
        std::cout << "Error receiving TCP packet...";
        return false;
    }

    memcpy(&packet, buffer, size);
    return true;
}

//http://www.sfml-dev.org/tutorials/1.6/network-sockets.php
/*
#define TCP_CONNECTION

enum ConnectionType
{
	TCP,
	UDP
};

struct ClientLogInPacket
{
	char username[32];
	char password[32];
};

struct ClientGamePacket
{
	unsigned int id;
	float x, y;
};

struct ServerLogInPacket
{
	bool confirm;
};

const unsigned short PORT = 8080;
const std::string IPADDRESS("127.0.0.1");

std::string msgSend;

sf::TcpSocket tcpSocket;
sf::UdpSocket udpSocket;

sf::Mutex globalMutex;
bool quit = false;
bool connected = false;

void GetInput(void)
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

	ClientGamePacket packet;
	packet.id = 1;
	packet.x = 128.5f;
	packet.y = 0.5f;

	//Sending Side
	char buffer[sizeof(packet)];
	memcpy(buffer, &packet, sizeof(packet));

	//Receiving Side
	ClientGamePacket tmp;
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

bool ClientConnect(ConnectionType type)
{
	if (type == TCP)
	{
		if (tcpSocket.connect(IPADDRESS, PORT) == sf::Socket::Done)
		{
			std::cout << "Connected.\n";
			return true;
		}
		else
			return false;
	}
	else
	{
		//UDP does not need a previous connection for sending
		udpSocket.setBlocking(false);
		//Bind to a local port to receive from server
		udpSocket.bind(sf::Socket::AnyPort);
		return true;
	}
}

void ClientDisconnect(ConnectionType type)
{
	if (type == TCP)
		tcpSocket.disconnect();
	else
		udpSocket.unbind();
}

bool SendPacket(ConnectionType type, ClientLogInPacket packet)
{
	// Create bytes to send
	const int size = sizeof(packet);
	char buffer[size];
	memcpy(buffer, &packet, size);

	if (type == TCP)
	{
		if (tcpSocket.send(buffer, size) != sf::Socket::Done)
		{
			std::cout << "Error sending TCP packet...";
			return false;
		}

		return true;
	}
	else
	{
		std::cout << "Error sending UDP packet...";
		return false;
	}
}

bool SendPacket(ConnectionType type, ClientGamePacket packet)
{
	if (type == TCP)
	{
		std::cout << "Error sending TCP packet...";
		return false;
	}
	else
	{
		std::cout << "Error sending UDP packet...";
		return false;
	}
}

bool ReceivePacket(ConnectionType type, ServerLogInPacket &packet)
{
	//Prepare buffer to receive
	const int size = sizeof(packet);
	char buffer[size];
	std::size_t Received;

	if (type == TCP)
	{
		//it is blocking until it receives data (can set the socket to non-blocking)
		if (tcpSocket.receive(buffer, size, Received) != sf::Socket::Done)
			std::cout << "Error receiving TCP packet..."; return false;

		memcpy(&packet, buffer, size);
		return true;
	}
	else
		std::cout << "Error receiving UDP packet..."; return false;
}

void LogIn()
{
	if (ClientConnect(TCP))
	{
		ClientLogInPacket logInPacket;

		std::string username, password;
		std::cout << "Enter username: ";
		std::cin >> username;

		std::cout << "Enter password: ";
		std::cin >> password;

		std::strcpy(logInPacket.username, username.c_str());
		std::strcpy(logInPacket.password, password.c_str());

		SendPacket(TCP, logInPacket);

		ServerLogInPacket serverLogIn;

		if (ReceivePacket(TCP, serverLogIn))
		{
			if (serverLogIn.confirm)
			{
				system("CLS");
				std::cout << "Loged in as: " << username;
				std::cin >> username;
				// joined wait room...
			}
			else
			{
				system("CLS");
				std::cout << "Username or password are incorrect.\nTry again.\n";
				LogIn();
			}
		}
	}
	else
	{
		std::cout << "Can't connect with server.\n";
		std::cout << "Retrying in 15 seconds...";
		sf::sleep(sf::milliseconds(5000));
		system("CLS");
		LogIn();
	}
}

int main() 
{
    //sf::Thread* thread = 0;

    //thread = new sf::Thread(&DoStuff);
    //thread->launch();

	//LogIn();

    return 0;
}
*/
