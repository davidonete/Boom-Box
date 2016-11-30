#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Object.h"

#pragma once

class GameManager
{
public:
  ~GameManager();

  static GameManager* GetInstance();

  void Init();

  void SpawnObject(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, char* texturePath, Vec2 spriteOrigin);

  void Input();
  void Update();
  void Render();

  inline b2World* GetWorld() { return world; }
  inline sf::RenderWindow* GetWindow() { return window; }
  inline void GetInput() { return; }

private:
  GameManager();

  sf::RenderWindow* window;
  b2World* world;

  static GameManager* Instance;

  std::vector<Object> SpawnedObjects;
};

#endif