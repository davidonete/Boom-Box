#include "Scenes/Scene.h"

Scene::Scene() {}

Scene::~Scene() 
{
    Objects.clear();
}

void Scene::Init() 
{
    World = new b2World(b2Vec2(0.0f, 9.8f));
}

void Scene::Input()
{
	for (unsigned int i = 0; i < Objects.size(); i++)
		Objects[i].Input();
}

void Scene::Update()
{
	for (unsigned int i = 0; i < Objects.size(); i++)
		Objects[i].Update();
}

void Scene::Render()
{
	for (unsigned int i = 0; i < Objects.size(); i++)
		Objects[i].Render();

	//Simulate the world 
	World->Step(1 / 60.0f, 8, 3);
}

void Scene::AddObject(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, const char * texturePath, Vec2 spriteOrigin)
{
	Objects.push_back(Object(position, scale, type, density, friction, texturePath, spriteOrigin));
}
