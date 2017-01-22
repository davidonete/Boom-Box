//
//  Player.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "System/NetworkManager.h"

class Player : public Object
{
public:
    Player(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world, unsigned int ID, bool bombStart);
    ~Player();

    void Init() override;
    void Input() override;
    void Update() override;
    void Render() override;

    inline unsigned int GetPlayerID() { return playerID; }
    inline bool HaveBomb() { return hasBomb; }
    void SetBomb(bool bomb);

private:
    void OnCollisionDetected(Object* otherObject) override;
    void UpdatePlayerServer();

    ObjectSprite bomb;
    ObjectSprite playerMark;

    unsigned int playerID;

    bool isPlayer;
    bool hasBomb;
    bool jumping;

    float32 speed = 6.0f;
    float32 jumpForce = 8.5f;

    GamePacket lastPacketSent;
};

#endif