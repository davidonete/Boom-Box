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

enum ConnectionMessage
{
    Connection_Refused,
    Connection_Accepted,
    Connection_Accepted_Authority,
    Connection_AlreadyLogged,
};

struct LogInPacket
{
    char username[32];
    char password[32];
};

struct LogOutPacket
{
    unsigned int ID;
};

struct GamePacket
{
    unsigned int id;
    float x, y;
};

struct ConfirmPacket
{
    unsigned int id;
    unsigned int msg;
};

struct ClientInfo
{
    unsigned int id;
    bool authority;
    std::string username;
};

struct ChatPacket
{
    unsigned int id;
    char message[50];
};

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    void Init();
    bool Connect(ConnectionType type);
    void Disconnect(ConnectionType type);

    bool SendPacket(LogInPacket packet);
    bool SendPacket(LogOutPacket packet);
    bool SendPacket(ChatPacket packet);
    bool SendPacket(ConnectionType type, GamePacket packet);

    bool ReceivePacket(ConnectionType Type, char* buffer);
    bool GetPacketFromBytes(char bytes[], ChatPacket &packet);
    bool GetPacketFromBytes(char bytes[], ConfirmPacket &packet);

    static inline ConnectionMessage GetConnectionMessage(unsigned int code) { return static_cast<ConnectionMessage>(code); }

    /** Gets and sets if the current client is the authority of the game. */
    inline void SetAuthority(bool authority) { client.authority = authority; }
    inline bool IsAuthority() { return client.authority; }

    /** Gets/Sets the id used by the server to identify this client. */
    inline unsigned int GetClientID() { return client.id; }
    inline void SetClientID(unsigned int id) { client.id = id; }
    inline std::string GetUsername() { return client.username; }
    inline void SetUsername(std::string username) { client.username = username; }

private:
    sf::TcpSocket tcpSocket;
    sf::UdpSocket udpSocket;

    ClientInfo client;
};

#endif