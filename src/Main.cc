#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Assets.h"
#include "GameManager.h"

#define SCALE 30 //pass from pixels to box2D positions

void Init()
{
  GameManager::GetInstance()->Init();

  GameManager::GetInstance()->SpawnObject(Vec2(200.0f, 20.0f), Vec2(32.0f, 32.0f), DynamicBody, 1.0f, 0.7f, "box.png", Vec2(16.0f, 16.0f));
  //GameManager::GetInstance()->SpawnObject(Vec2(200.0f, 20.0f), Vec2(32.0f, 32.0f), DynamicBody, 1.0f, 0.7f, "box.png", Vec2(16.0f, 16.0f));
  GameManager::GetInstance()->SpawnObject(Vec2(400.0f, 500.0f), Vec2(800.0f, 16.0f), StaticBody, 0.0f, 5.0f, "ground.png", Vec2(400.0f, 8.0f));
}

void Remove()
{
  //To Do: Remove GameManager
}

void Input(GameManager* GM)
{
  GM->Input();
}

void Update(GameManager* GM)
{
  sf::Event event;
  while (GM->GetWindow()->pollEvent(event))
  {
    // Close window: exit
    if (event.type == sf::Event::Closed)
      GM->GetWindow()->close();
  }

  GM->Update();
}

void Render(GameManager* GM)
{
  GM->Render();
}

int main() 
{
  GameManager* GM = GameManager::GetInstance();

  Init();
  while (GM->GetWindow()->isOpen())
  {
    Input(GM);
    Update(GM);
    Render(GM);
  }
  return EXIT_SUCCESS;
}

//http://www.iforce2d.net/b2dtut/constant-speed