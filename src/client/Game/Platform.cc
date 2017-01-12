#include "Game/Platform.h"
#include "System/GameManager.h"

Platform::Platform(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world) 
{
    GM = GameManager::GetInstance();
    Window = GM->GetWindow();

    Vec2 Scale(286.0f, 70.0f);
    SpriteOrigin = Vec2(Scale.x / 2.0f, Scale.y / 2.0f);
    Texture.loadFromFile(GameManager::GetImagePath("platform.png"));

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(position.x / SCALE, position.y / SCALE);
    BodyDef.type = b2_staticBody;

    Body = world->CreateBody(&BodyDef);

    SetRotaiton(rotation);

    //Upper part
    b2Vec2 vertices[4];
    vertices[0].Set((5 - SpriteOrigin.x) / SCALE, (0 - SpriteOrigin.y) / SCALE);
    vertices[1].Set((280 - SpriteOrigin.x) / SCALE, (0 - SpriteOrigin.y) / SCALE);
    vertices[2].Set((284 - SpriteOrigin.x) / SCALE, (4 - SpriteOrigin.y) / SCALE);
    vertices[3].Set((2 - SpriteOrigin.x) / SCALE, (4 - SpriteOrigin.y) / SCALE);

    // Creates a box shape. Divide your desired width and height by 2.
    b2PolygonShape Shape;
    Shape.Set(vertices, 4);

    b2FixtureDef FixtureDef;
    FixtureDef.density = density;
    FixtureDef.friction = friction;
    FixtureDef.shape = &Shape;
    Body->CreateFixture(&FixtureDef);

    //Bottom part
    b2Vec2 vertices2[8];
    vertices2[0].Set((5 - SpriteOrigin.x) / SCALE, (1 - SpriteOrigin.y) / SCALE);
    vertices2[1].Set((280 - SpriteOrigin.x) / SCALE, (1 - SpriteOrigin.y) / SCALE);
    vertices2[2].Set((286 - SpriteOrigin.x) / SCALE, (17 - SpriteOrigin.y) / SCALE);
    vertices2[3].Set((273 - SpriteOrigin.x) / SCALE, (35 - SpriteOrigin.y) / SCALE);
    vertices2[4].Set((213 - SpriteOrigin.x) / SCALE, (65 - SpriteOrigin.y) / SCALE);
    vertices2[5].Set((75 - SpriteOrigin.x) / SCALE, (66 - SpriteOrigin.y) / SCALE);
    vertices2[6].Set((12 - SpriteOrigin.x) / SCALE, (35 - SpriteOrigin.y) / SCALE);
    vertices2[7].Set((1 - SpriteOrigin.x) / SCALE, (18 - SpriteOrigin.y) / SCALE);

    b2PolygonShape Shape2;
    Shape2.Set(vertices2, 8);

    b2FixtureDef FixtureDef2;
    FixtureDef2.density = density;
    FixtureDef2.friction = 0.0f;
    FixtureDef2.shape = &Shape2;
    Body->CreateFixture(&FixtureDef2);
}

Platform::~Platform() {}

void Platform::Init() {}

void Platform::Input() {}

void Platform::Update()
{
    Object::Update();
}
