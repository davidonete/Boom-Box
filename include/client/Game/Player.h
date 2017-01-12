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

class Player : public Object
{
public:
    Player(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world, bool localPlayer);
    ~Player();

    void Init() override;
    void Input() override;
    void Update() override;

private:

};

#endif