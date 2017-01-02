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

struct ServerConfirmPacket
{
    unsigned int id;
    bool accepted;
    bool authority;
};

struct ClientInfo
{
    unsigned int id;
    bool authority;
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

    bool ReceivePacket(ConnectionType type, ServerConfirmPacket &packet);

    /** Gets and sets if the current client is the authority of the game. */
    inline void SetAuthority(bool authority) { client.authority = authority; }
    inline bool IsAuthority() { return client.authority; }

    /** Gets/Sets the id used by the server to identify this client. */
    inline unsigned int GetClientID() { return client.id; }
    inline void SetClientID(unsigned int id) { client.id = id; }

private:
    sf::TcpSocket tcpSocket;
    sf::UdpSocket udpSocket;

    ClientInfo client;
};

#endif