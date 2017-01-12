//
//  Object.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#pragma once

class GameManager;

//Pass from pixels to box2D coords
#define SCALE 30.0f 

struct Vec2
{
  float32 x;
  float32 y;
  Vec2()
  {
      x = 0.0f;
      y = 0.0f;
  }
  Vec2(float32 _x, float32 _y)
  {
    x = _x;
    y = _y;
  }
};

enum ObjectType
{
  StaticBody,
  DynamicBody
};

struct ObjectParameters
{
  Vec2 position;
  Vec2 scale;
  ObjectType type;
  float32 density;
  float32 friction;
  char* texturePath;
  Vec2 spriteOrigin;
};

class Object
{
public:
  Object() {}
  // Only for square shapes
  Object(Vec2 position, Vec2 scale, float32 rotation, ObjectType type, float32 density, float32 friction, const char* texturePath, Vec2 spriteOrigin, b2World* world);
  ~Object();

  virtual void Init();
  virtual void Input();
  virtual void Update();
  void Render();

protected:
  Vec2 GetPosition();
  float32 GetRotation();

  void SetPosition(Vec2 pos);
  void SetRotaiton(float32 angle);

  b2Body* Body;
  sf::Texture Texture;
  Vec2 SpriteOrigin;

  GameManager* GM;
  sf::RenderWindow* Window;

private:
  sf::Sprite LastUpdatedSprite;

};

#endif
