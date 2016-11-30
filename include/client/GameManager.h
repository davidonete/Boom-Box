#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Object.h"

#pragma once

enum InputData
{
  InputData_LeftPressed,
  InputData_RightPressed,
  InputData_SpacePressed
};

class GameManager
{
public:
  ~GameManager();

  static GameManager* GetInstance();

  void Init();

  void SpawnObject(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, const char* texturePath, Vec2 spriteOrigin);

  void Input();
  void Update();
  void Render();
    
  bool CheckInputPressed(InputData InputType);

  inline b2World* GetWorld() { return world; }
  inline sf::RenderWindow* GetWindow() { return window; }

private:
  GameManager();
  void UpdateInput();

  sf::RenderWindow* window;
  b2World* world;

  static GameManager* Instance;

  std::vector<Object*> SpawnedObjects;
  std::vector<InputData> LastInputPressed;
};

#endif
