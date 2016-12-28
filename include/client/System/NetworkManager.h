//
//  NetworkManager.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "SFML/Network.hpp"

enum ConnectionType
{
    TCP,
    UDP
};

struct LogInPacket
{
    char username[32];
    char password[32];
};

struct GamePacket
{
    unsigned int id;
    float x, y;
};

struct ServerLogInPacket
{
    bool confirm;
};

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    void Init();
    bool Connect(ConnectionType type);
    void Disconnect(ConnectionType type);

    bool SendPacket(ConnectionType type, LogInPacket packet);
    bool SendPacket(ConnectionType type, GamePacket packet);

    bool ReceivePacket(ConnectionType type, ServerLogInPacket &packet);

private:
    sf::TcpSocket tcpSocket;
    sf::UdpSocket udpSocket;

};

#endif