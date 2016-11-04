#include "GameManager.h"

GameManager* GameManager::Instance = nullptr;

GameManager::GameManager()
{

}

GameManager::~GameManager()
{

}

GameManager* GameManager::GetInstance()
{
  if(Instance == nullptr)
    Instance = new GameManager();

  return Instance;
}

void GameManager::Init()
{
  //Initialize window (width, height, bits per pixel)
  window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "SFML window");
  window->setFramerateLimit(60);

  //Initialize world with gravity value
  world = new b2World(b2Vec2(0.0f, 9.8f));
}

void GameManager::SpawnObject(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, const char* texturePath, Vec2 spriteOrigin)
{
  SpawnedObjects.push_back(new Object(position, scale, type, density, friction, texturePath, spriteOrigin));
}

void GameManager::Input()
{
  UpdateInput();
  for (int i = 0; i < SpawnedObjects.size(); i++)
    SpawnedObjects[i]->Input();
}

void GameManager::Update()
{
  for (int i = 0; i < SpawnedObjects.size(); i++)
    SpawnedObjects[i]->Update();
}

void GameManager::Render()
{
  window->clear(sf::Color::White);

  for (int i = 0; i < SpawnedObjects.size(); i++)
    SpawnedObjects[i]->Render();

  //Simulate the world 
  world->Step(1 / 60.0f, 8, 3);

  //Update the window
  window->display();
}

void GameManager::UpdateInput()
{
    LastInputPressed.clear();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        LastInputPressed.push_back(InputData_RightPressed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        LastInputPressed.push_back(InputData_LeftPressed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        LastInputPressed.push_back(InputData_SpacePressed);
}


bool GameManager::CheckInputPressed(InputData InputType)
{
    for(int i = 0; i < LastInputPressed.size(); i++)
    {
        if(LastInputPressed[i] == InputType)
            return true;
    }
    return false;
}
