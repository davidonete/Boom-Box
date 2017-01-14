#include "Scenes/Scene.h"

Scene::Scene() {}

Scene::~Scene() 
{
    Objects.clear();
}

void Scene::Init() {}

void Scene::Input()
{
	for (unsigned int i = 0; i < Objects.size(); i++)
		Objects[i]->Input();
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
}

void Scene::AddPlayer(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world, bool localPlayer)
{
    players.push_back(new Player(position, rotation, density, friction, world, localPlayer));
    Objects.push_back(players.back());
}

void Scene::AddPlatform(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world)
{
    Objects.push_back(new Platform(position, rotation, density, friction, world));
}

void Scene::AddObject(Vec2 position, Vec2 scale, float rotation, ObjectType type, float32 density, float32 friction, const char * texturePath, b2World * world)
{
    Objects.push_back(new Object(position, scale, rotation, type, density, friction, texturePath, world));
}