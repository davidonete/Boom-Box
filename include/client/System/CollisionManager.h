//
//  CollisionManager.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright � 2016 David Parra. All rights reserved.
//

#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <Box2D/Box2D.h>

/** Collision Manager.
 *  Handles the collisions between all the objects of the game.
 */
class CollisionManager : public b2ContactListener
{
public:
    CollisionManager();
    ~CollisionManager();

   /** Called when two objects collide.
    *  @param contact The information about the collision (we can find the objects who collide inside)
    */
    void BeginContact(b2Contact* contact) override;

private:

};

#endif
