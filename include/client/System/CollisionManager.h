//
//  CollisionManager.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <Box2D/Box2D.h>

class CollisionManager : public b2ContactListener
{
public:
    CollisionManager();
    ~CollisionManager();

    void BeginContact(b2Contact* contact) override;

private:

};

#endif