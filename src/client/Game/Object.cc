#include "Game/Object.h"
#include "System/GameManager.h"

//Pass from pixels to box2D coords
#define SCALE 30.0f 

//http://www.iforce2d.net/b2dtut/constant-speed
Object::Object(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, const char* texturePath, Vec2 spriteOrigin)
{
    GM = GameManager::GetInstance();
    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(position.x / SCALE, position.y / SCALE);

    if (type == StaticBody)
        BodyDef.type = b2_staticBody;
    else if (type == DynamicBody)
        BodyDef.type = b2_dynamicBody;

    //Body = GameManager::GetInstance()->GetWorld()->CreateBody(&BodyDef);

    // Creates a box shape. Divide your desired width and height by 2.
    b2PolygonShape Shape;
    Shape.SetAsBox((scale.x / 2.0f) / SCALE, (scale.y / 2.0f) / SCALE);

    b2FixtureDef FixtureDef;
    FixtureDef.density = density;
    FixtureDef.friction = friction;
    FixtureDef.shape = &Shape;
    Body->CreateFixture(&FixtureDef);

    Texture.loadFromFile(GameManager::GetImagePath(texturePath));

    SpriteOrigin = b2Vec2(spriteOrigin.x, spriteOrigin.y);
}

Object::~Object()
{

}

void Object::Init()
{

}

void Object::Input()
{

}

void Object::Update()
{
  sf::Sprite Sprite;
  Sprite.setTexture(Texture);

  // ???
  Sprite.setOrigin(SpriteOrigin.x, SpriteOrigin.y);

  Vec2 pos = GetPosition();
  Sprite.setPosition(pos.x, pos.y);

  Sprite.setRotation(GetRotation());

  //Linear velocity
  //Body->SetLinearVelocity(b2Vec2(inputValue, 0.0f));

  //Impulse
  /*
  b2Vec2 vel = Body->GetLinearVelocity();
  float inputValue = 5.0f;
  float velChange = inputValue - vel.x;
  float impulse = Body->GetMass() * velChange;
  Body->ApplyLinearImpulse(b2Vec2(impulse, 0), Body->GetWorldCenter(), true);
  */

  //Force
  if (GM->CheckInputPressed(InputData_RightPressed))
  {
    b2Vec2 vel = Body->GetLinearVelocity();
    float velChange = 1.0f - vel.x;
    float32 force = Body->GetMass() * velChange / (1.0f / 60.0f); //f = mv/t
    Body->ApplyForce(b2Vec2(force, 0), Body->GetWorldCenter(), true);
  }

  if (GM->CheckInputPressed(InputData_LeftPressed))
  {
    b2Vec2 vel = Body->GetLinearVelocity();
    float velChange = -1.0f - vel.x;
    float32 force = Body->GetMass() * velChange / (1.0f / 60.0f); //f = mv/t
    Body->ApplyForce(b2Vec2(force, 0), Body->GetWorldCenter(), true);
  }

  if (GM->CheckInputPressed(InputData_SpacePressed))
  {
    b2Vec2 vel = Body->GetLinearVelocity();
    float inputValue = 5.0f;
    float velChange = -inputValue - vel.y;
    float impulse = Body->GetMass() * velChange;
    Body->ApplyLinearImpulse(b2Vec2(0.0f, impulse), Body->GetWorldCenter(), true);
  }

  LastUpdatedSprite = Sprite;
}

void Object::Render()
{
  GM->GetWindow()->draw(LastUpdatedSprite);
}

Vec2 Object::GetPosition()
{
  // ???
  return Vec2(Body->GetPosition().x * SCALE, Body->GetPosition().y * SCALE);
}

float32 Object::GetRotation()
{
  // ???
  float32 result;
  if (Body->GetType() == b2_staticBody)
    result = (180 / b2_pi * Body->GetAngle());
  else if (Body->GetType() == b2_dynamicBody)
    result = (Body->GetAngle() * 180 / b2_pi);

  return result;
}


void Object::SetPosition(Vec2 pos)
{
  //TODO
}

void Object::SetRotaiton(Vec2 rot)
{
  //TODO
}
