#include "Game/Object.h"
#include "System/GameManager.h"

//http://www.iforce2d.net/b2dtut/constant-speed
Object::Object(Vec2 position, Vec2 scale, float rotation, ObjectType type, float32 density, float32 friction, const char* texturePath, Vec2 spriteOrigin, b2World* world)
{
    GM = GameManager::GetInstance();
    Window = GM->GetWindow();

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(position.x / SCALE, position.y / SCALE);

    if (type == StaticBody)
        BodyDef.type = b2_staticBody;
    else if (type == DynamicBody)
        BodyDef.type = b2_dynamicBody;

    Body = world->CreateBody(&BodyDef);

    SetRotaiton(rotation);

    // Creates a box shape. Divide your desired width and height by 2.
    b2PolygonShape Shape;

    Shape.SetAsBox((scale.x / 2.0f) / SCALE, (scale.y / 2.0f) / SCALE);
    b2FixtureDef FixtureDef;
    FixtureDef.density = density;
    FixtureDef.friction = friction;
    FixtureDef.shape = &Shape;
    Body->CreateFixture(&FixtureDef);

    Texture.loadFromFile(GameManager::GetImagePath(texturePath));
    SpriteOrigin = Vec2(spriteOrigin.x, spriteOrigin.y);
}

Object::~Object() {}

void Object::Init() {}

void Object::Input() {}

void Object::Update()
{
    sf::Sprite Sprite;
    Sprite.setTexture(Texture);

    Sprite.setOrigin(SpriteOrigin.x, SpriteOrigin.y);

    Vec2 pos = GetPosition();
    Sprite.setPosition(pos.x, pos.y);

    Sprite.setRotation(GetRotation());
    LastUpdatedSprite = Sprite;
}

void Object::Render()
{
    Window->draw(LastUpdatedSprite);
}

Vec2 Object::GetPosition()
{
    return Vec2(Body->GetPosition().x * SCALE, Body->GetPosition().y * SCALE);
}

float32 Object::GetRotation()
{
    float32 result;
    if (Body->GetType() == b2_staticBody)
        result = (180 / b2_pi * Body->GetAngle());
    else if (Body->GetType() == b2_dynamicBody)
        result = (Body->GetAngle() * 180 / b2_pi);

    return result;
}

void Object::SetPosition(Vec2 pos)
{
    Body->SetTransform(b2Vec2(pos.x / SCALE, pos.y / SCALE), Body->GetAngle());
}

void Object::SetRotaiton(float32 angle)
{
    Body->SetTransform(Body->GetPosition(), angle / (180 / b2_pi));
}