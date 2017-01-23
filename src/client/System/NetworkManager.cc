#include "System/NetworkManager.h"

#include <iostream>

NetworkManager::NetworkManager() {}

NetworkManager::~NetworkManager() {}

void NetworkManager::Init()
{
    client.authority = false;
    client.ID = 0;
}

bool NetworkManager::Connect(ConnectionType type)
{
    if (type == TCP)
    {
        sf::Socket::Status socket = tcpSocket.connect(IPADDRESS, PORT, sf::Time(sf::seconds(5)));
        if (socket == sf::Socket::Done)
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
        //Bind to a local port to receive from server
        udpSocket.bind(sf::Socket::AnyPort);
        return true;
    }
}

void NetworkManager::Disconnect(ConnectionType type)
{
    if (type == TCP)
        tcpSocket.disconnect();
    else
        udpSocket.unbind();
}

bool NetworkManager::SendPacket(GamePacket packet)
{
    const int size = sizeof(packet);
    char buffer[size];
    memcpy(buffer, &packet, size);

    if (udpSocket.send(buffer, size, IPADDRESS, PORT) != sf::Socket::Done)
    {
        std::cout << "Error sending UDP packet...";
        return false;
    }
    return true;
}

bool NetworkManager::SendPacket(ClientRequestPacket packet)
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

bool NetworkManager::SendPacket(PlayerBombChangePacket packet)
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

unsigned long NetworkManager::ReceivePacket(ConnectionType Type, char buffer[])
{
    //Prepare buffer to receive
    const int size = 256;
    std::size_t Received;

    if (Type == TCP)
    {
        //it is blocking until it receives data (can set the socket to non-blocking)
        if (tcpSocket.receive(buffer, size, Received) != sf::Socket::Done)
        {
            std::cout << "Error receiving TCP packet...";
            return 0;
        }
        return Received;
    }
    else
    {
        sf::IpAddress Sender;
        unsigned short Port;

        if (udpSocket.receive(buffer, size, Received, Sender, Port) != sf::Socket::Done)
        {
            std::cout << "Error receiving UDP packet...";
            return 0;
        }
        return Received;
    }
}

void NetworkManager::LogOut()
{
    ClientRequestPacket packet;
    packet.ID = GetClientID();
    packet.msg = GetCodeFromRequest(Request_DisconnectPlayer);
    SendPacket(packet);
}

PacketType NetworkManager::GetPacketType(unsigned long size)
{
    switch (size)
    {
        case sizeof(GamePacket) :
            return Type_GamePacket;
        case sizeof(ChatPacket) :
            return Type_ChatPacket;
        case sizeof(PlayerInfoPacket) :
            return Type_PlayerInfoPacket;
        case sizeof(ServerMessagePacket) :
            return Type_ServerMessagePacket;
        case sizeof(ClientRequestPacket) :
            return Type_ClientRequestPacket;
        case sizeof(ServerChangeBombPacket) :
            return Type_ServerChangeBombPacket;
    }
    return Type_Null;
}

void NetworkManager::GetPacketFromBytes(char bytes[], PlayerInfoPacket &packet)
{
    memcpy(&packet, bytes, sizeof(packet));
}

void NetworkManager::GetPacketFromBytes(char bytes[], GamePacket &packet)
{
    memcpy(&packet, bytes, sizeof(packet));
}

void NetworkManager::GetPacketFromBytes(char bytes[], ServerChangeBombPacket &packet)
{
    memcpy(&packet, bytes, sizeof(packet));
}

void NetworkManager::GetPacketFromBytes(char bytes[], ChatPacket &packet)
{
    memcpy(&packet, bytes, sizeof(packet));
}

void NetworkManager::GetPacketFromBytes(char bytes[], ServerMessagePacket &packet)
{
    memcpy(&packet, bytes, sizeof(packet));
}

void NetworkManager::GetPacketFromBytes(char bytes[], ClientRequestPacket &packet)
{
    memcpy(&packet, bytes, sizeof(packet));
}
