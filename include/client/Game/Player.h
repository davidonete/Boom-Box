//
//  Player.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright � 2016 David Parra. All rights reserved.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "System/NetworkManager.h"

/** Player.
 *  The object class that can be controlled by the user.
 */
class Player : public Object
{
public:
    Player(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world, unsigned int ID, bool bombStart);
    ~Player();

    void Init() override;
    void Input() override;
    void Update() override;
    void Render() override;

    /** Gets the player ID.
     *  @return The player ID.
     */
    inline unsigned int GetPlayerID() { return playerID; }
    
    /** Gets if the player has the bomb.
     *  @return True if the player has the bomb.
     */
    inline bool HaveBomb() { return hasBomb; }
    /** Sets if the player has the bomb.
     *  @param bomb If the player has the bomb.
     */
    void SetBomb(bool bomb);

private:
    /** Called whenever the player collides with another object.
     *  @param otherObject The other object.
     */
    void OnCollisionDetected(Object* otherObject) override;
    /** Called every step to send the player information to the server */
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
