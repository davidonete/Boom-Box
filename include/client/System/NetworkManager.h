//
//  NetworkManager.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright � 2016 David Parra. All rights reserved.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "SFML/Network.hpp"

enum ConnectionType
{
  TCP,
  UDP
};

enum ServerMessage
{
  Server_Denied,
  Server_Denied_NotAuthority,
  Server_Denied_AlreadyLogged,
  Server_Denied_NotEnoughPlayers,
  Server_Accepted,
  Server_AcceptedAuthority,
  Server_PlayerConnected,
  Server_PlayerDisconnected,
  Server_StartBattleScene,
  Server_StartBattle,
  Server_BombPossession,
  Server_PlayerDead,
};

enum RequestMessage
{
  Request_GetPlayersInfo,
  Request_DisconnectPlayer,
  Request_StartBattleScene,
  Request_PlayerReady,
  Request_TimeOver,
  Request_PlayerChangeBomb,
};

enum PacketType
{
  Type_Null,
  Type_GamePacket,
  Type_ChatPacket,
  Type_ServerMessagePacket,
  Type_PlayerInfoPacket,
  Type_ClientRequestPacket,
  Type_ServerChangeBombPacket,
};

struct LogInPacket
{
  char username[32];
  char password[32];
};

struct GamePacket
{
  unsigned int ID;
  float x, y, rotation;
};

struct PlayerInfoPacket
{
  unsigned int ID;
  unsigned int wincount;
  unsigned int authority;
  char username[32];
};

struct ChatPacket
{
  unsigned int ID;
  char message[50];
};

struct ServerMessagePacket
{
  unsigned int msg;
};

struct ServerChangeBombPacket
{
  unsigned int fromID;
  unsigned int toID;
  unsigned int msg;
};

struct ClientRequestPacket
{
  unsigned int ID;
  unsigned int msg;
};

struct PlayerBombChangePacket
{
  unsigned int ID;
  unsigned int otherID;
  unsigned int msg;
};

struct PlayerInformation
{
  unsigned int ID;
  unsigned int wincount;
  bool authority;
  std::string username;
};

/** Network Manager.
 *  Has all the networking functionality of the game.
 */
class NetworkManager
{
public:
  NetworkManager();
  ~NetworkManager();

  /** Initializes all the parameters needed to work with the network */
  void Init();
    
  /** Connects the client to a specific server.
   *  @param type The connection type (TCP/UDP)
   */
  bool Connect(ConnectionType type);
    
  /** Disconnects the client from a previous server connection.
   *  @param type The connection type (TCP/UDP)
   */
  void Disconnect(ConnectionType type);

  /** Tries to send a packet to the server (needs a TCP connection)
   *  @param packet The packet to be sent.
   *  @return True if the packet is correctly sent.
   */
  bool SendPacket(LogInPacket packet);
    
  /** Tries to send a packet to the server (needs a TCP connection)
   *  @param packet The packet to be sent.
   *  @return True if the packet is correctly sent.
   */
  bool SendPacket(ChatPacket packet);
    
  /** Tries to send a packet to the server (needs a TCP connection)
   *  @param packet The packet to be sent.
   *  @return True if the packet is correctly sent.
   */
  bool SendPacket(ClientRequestPacket packet);
    
  /** Tries to send a packet to the server (needs a TCP connection)
   *  @param packet The packet to be sent.
   *  @return True if the packet is correctly sent.
   */
  bool SendPacket(PlayerBombChangePacket packet);
    
  /** Tries to send a packet to the server (needs a UDP connection)
   *  @param packet The packet to be sent.
   *  @return True if the packet is correctly sent.
   */
  bool SendPacket(GamePacket packet);

  /** Tries to receive a packet from the server (needs a TCP/UDP connection)
   *  @param Type The type of connection from where receive packet.
   *  @param buffer The buffer where the data received is going to be stored.
   *  @return The amount of bytes received from the server (it can be 0 when no data is received).
   */
  unsigned long ReceivePacket(ConnectionType Type, char buffer[]);
    
  /** Gets the type of packet depending of the size.
   *  @param size The size (in bytes) of the packet
   *  @return The packet type (if no packet type is detected it returns Type_Null)
   */
  static PacketType GetPacketType(unsigned long size);
    
  /** Transforms a array of bytes to a specific packet type (before using this make sure to get the type of packet (GetPacketType)).
   *  @param bytes The buffer where the data is stored.
   *  @param packet The struct where the data is going to be transformed from the bytes.
   */
  void GetPacketFromBytes(char bytes[], ChatPacket &packet);
    
  /** Transforms a array of bytes to a specific packet type (before using this make sure to get the type of packet (GetPacketType)).
   *  @param bytes The buffer where the data is stored.
   *  @param packet The struct where the data is going to be transformed from the bytes.
   */
  void GetPacketFromBytes(char bytes[], ServerMessagePacket &packet);
    
  /** Transforms a array of bytes to a specific packet type (before using this make sure to get the type of packet (GetPacketType)).
   *  @param bytes The buffer where the data is stored.
   *  @param packet The struct where the data is going to be transformed from the bytes.
   */
  void GetPacketFromBytes(char bytes[], ClientRequestPacket &packet);
    
  /** Transforms a array of bytes to a specific packet type (before using this make sure to get the type of packet (GetPacketType)).
   *  @param bytes The buffer where the data is stored.
   *  @param packet The struct where the data is going to be transformed from the bytes.
   */
  void GetPacketFromBytes(char bytes[], PlayerInfoPacket &packet);
  
  /** Transforms a array of bytes to a specific packet type (before using this make sure to get the type of packet (GetPacketType)).
   *  @param bytes The buffer where the data is stored.
   *  @param packet The struct where the data is going to be transformed from the bytes.
   */
  void GetPacketFromBytes(char bytes[], GamePacket &packet);
  
  /** Transforms a array of bytes to a specific packet type (before using this make sure to get the type of packet (GetPacketType)).
   *  @param bytes The buffer where the data is stored.
   *  @param packet The struct where the data is going to be transformed from the bytes.
   */
  void GetPacketFromBytes(char bytes[], ServerChangeBombPacket &packet);
    
  /** Gets the server message type from the server message code received.
   *  @param code The server message code received.
   *  @return The server message enum getted from the code message.
   */
  static inline ServerMessage GetServerMessage(unsigned int code) { return static_cast<ServerMessage>(code); }
    
  /** Gets the request message code from the request enum.
   *  @param msg The enum request message.
   *  @return The code from the enum request message.
   */
  static inline unsigned int GetCodeFromRequest(RequestMessage msg) { return static_cast<unsigned int>(msg); }

  /** Sets if the current client is the authority of the game. */
  inline void SetAuthority(bool authority) { client.authority = authority; }
  /** Gets if the current client is the authority of the game. */
  inline bool IsAuthority() { return client.authority; }

  /** Gets the id used by the server to identify this client. */
  inline unsigned int GetClientID() { return client.ID; }
  /** Sets the id used by the server to identify this client. */
  inline void SetClientID(unsigned int id) { client.ID = id; }
    
  /** Gets the player username used by the server to identify this client. */
  inline std::string GetUsername() { return client.username; }
  /** Gets the player username used by the server to identify this client. */
  inline void SetUsername(std::string username) { client.username = username; }
    
  /** Gets the used server IP. */
  inline std::string GetIP() { return IPADDRESS; }
  /** Gets the used server Port. */
  inline unsigned short GetPort() { return PORT; }
  /** Sets the used server IP and Port. */
  inline void SetIP(std::string IP, unsigned int port) { IPADDRESS = IP; PORT = port; }
    
  /** Sends a log out message to the server. */
  void LogOut();

private:
  sf::TcpSocket tcpSocket;
  sf::UdpSocket udpSocket;

  std::string IPADDRESS = "152.105.5.136";
  unsigned short PORT = 8080;

  PlayerInformation client;
};

#endif
