#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#pragma once

struct Vec2
{
  float32 x;
  float32 y;
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
  // Only for square shapes
  Object(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, char* texturePath, Vec2 spriteOrigin);
  ~Object();

  virtual void Init();
  virtual void Input();
  virtual void Update();
  void Render();

protected:
  sf::Texture Texture;

  Vec2 GetPosition();
  float32 GetRotation();

  void SetPosition(Vec2 pos);
  void SetRotaiton(Vec2 rot);

private:
  b2Body* Body;
  sf::Sprite LastUpdatedSprite;

  b2Vec2 SpriteOrigin;
};

#endif