#include "System/CollisionManager.h"
#include "Game/Object.h"

CollisionManager::CollisionManager() {}

CollisionManager::~CollisionManager() {}

void CollisionManager::BeginContact(b2Contact* contact)
{
    Object* obj1;
    Object* obj2;

    void* data1 = contact->GetFixtureA()->GetBody()->GetUserData();
    void* data2 = contact->GetFixtureB()->GetBody()->GetUserData();
    if (data1 && data2)
    {
        obj1 = static_cast<Object*>(data1);
        obj2 = static_cast<Object*>(data2);

        if(obj1->GetIsCollisionEnabled())
            obj1->OnCollisionDetected(obj2);
        if (obj2->GetIsCollisionEnabled())
            obj2->OnCollisionDetected(obj1);
    }
}
