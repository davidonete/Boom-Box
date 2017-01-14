#include "Game/Platform.h"
#include "System/GameManager.h"

Platform::Platform(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world) 
{
    ObjectInfo* object = GetObject();

    Vec2 Scale(225.0f, 55.0f);
    object->sprite.spriteOrigin = Vec2(Scale.x / 2.0f, Scale.y / 2.0f);
    object->sprite.texture.loadFromFile(GameManager::GetImagePath("platform.png"));

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(position.x / SCALE, position.y / SCALE);
    BodyDef.type = b2_staticBody;

    object->body = world->CreateBody(&BodyDef);

    SetRotaiton(rotation);

    //Upper part
    b2Vec2 vertices[4];
    vertices[0].Set((4 - object->sprite.spriteOrigin.x) / SCALE, (0 - object->sprite.spriteOrigin.y) / SCALE);
    vertices[1].Set((220 - object->sprite.spriteOrigin.x) / SCALE, (0 - object->sprite.spriteOrigin.y) / SCALE);
    vertices[2].Set((223 - object->sprite.spriteOrigin.x) / SCALE, (3 - object->sprite.spriteOrigin.y) / SCALE);
    vertices[3].Set((1.5f - object->sprite.spriteOrigin.x) / SCALE, (3 - object->sprite.spriteOrigin.y) / SCALE);

    // Creates a box shape. Divide your desired width and height by 2.
    b2PolygonShape Shape;
    Shape.Set(vertices, 4);

    b2FixtureDef FixtureDef;
    FixtureDef.density = density;
    FixtureDef.friction = friction;
    FixtureDef.shape = &Shape;
    object->body->CreateFixture(&FixtureDef);

    //Bottom part
    b2Vec2 vertices2[8];
    vertices2[0].Set((4 - object->sprite.spriteOrigin.x) / SCALE, (1 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[1].Set((220 - object->sprite.spriteOrigin.x) / SCALE, (1 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[2].Set((225 - object->sprite.spriteOrigin.x) / SCALE, (13 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[3].Set((215 - object->sprite.spriteOrigin.x) / SCALE, (27 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[4].Set((167 - object->sprite.spriteOrigin.x) / SCALE, (51 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[5].Set((59 - object->sprite.spriteOrigin.x) / SCALE, (52 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[6].Set((9 - object->sprite.spriteOrigin.x) / SCALE, (27 - object->sprite.spriteOrigin.y) / SCALE);
    vertices2[7].Set((1 - object->sprite.spriteOrigin.x) / SCALE, (14 - object->sprite.spriteOrigin.y) / SCALE);

    b2PolygonShape Shape2;
    Shape2.Set(vertices2, 8);

    b2FixtureDef FixtureDef2;
    FixtureDef2.density = density;
    FixtureDef2.friction = 0.0f;
    FixtureDef2.shape = &Shape2;
    object->body->CreateFixture(&FixtureDef2);
}

Platform::~Platform() {}

void Platform::Init() {}

void Platform::Input() {}

void Platform::Update()
{
    Object::Update();
}
