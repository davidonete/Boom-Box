#include "Scenes/Scene.h"

Scene::Scene() {}

Scene::~Scene() 
{
    Objects.clear();
    if(World != nullptr)
        delete World;
}

void Scene::Init() {}

void Scene::Input()
{
    if (windowFocus)
    {
        for (unsigned int i = 0; i < Objects.size(); i++)
            Objects[i]->Input();
    }
}

void Scene::Update()
{
	for (unsigned int i = 0; i < Objects.size(); i++)
		Objects[i]->Update();
}

void Scene::Render()
{
	for (unsigned int i = 0; i < Objects.size(); i++)
		Objects[i]->Render();

	//Simulate the world 
	World->Step(1 / 60.0f, 8, 3);

    //Destroy objects requested after step
    for (int i = 0; i < objectsToDestroy.size(); i++)
    {
        //World->DestroyBody(objectsToDestroy[i].object->GetBody());
        Objects.erase(Objects.begin() + objectsToDestroy[i].index);
    }
    objectsToDestroy.clear();
}

void Scene::AddPlayer(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world, unsigned int ID, bool hasBomb)
{
    Objects.push_back(std::unique_ptr<Player> (new Player(position, rotation, density, friction, world, ID, hasBomb)));
}

void Scene::AddPlatform(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world)
{
    Objects.push_back(std::unique_ptr<Platform> (new Platform(position, rotation, density, friction, world)));
}

void Scene::AddObject(Vec2 position, Vec2 scale, float rotation, BodyType type, float32 density, float32 friction, const char * texturePath, b2World * world)
{
    Objects.push_back(std::unique_ptr<Object> (new Object(position, scale, rotation, type, density, friction, texturePath, world)));
}

void Scene::DestroyPlayer(unsigned int ID)
{
    for (int i = 0; i < Objects.size(); i++)
    {
        if (Objects[i]->GetType() == Type_Player)
        {
            Player *player = dynamic_cast<Player*>(Objects[i].get());
            if (player->GetPlayerID() == ID)
            {
                ObjectsToDestroy object;
                object.index = i;
                object.object = Objects[i].get();
                objectsToDestroy.push_back(object);
            }
        }
    }
}