#include "Game/Player.h"
#include "System/GameManager.h"

Player::Player(Vec2 position, float32 rotation, float32 density, float32 friction, b2World * world, bool localPlayer)
{
    GM = GameManager::GetInstance();
    Window = GM->GetWindow();

    Vec2 Scale(32.0f, 32.0f);
    SpriteOrigin = Vec2(Scale.x / 2.0f, Scale.y / 2.0f);
    Texture.loadFromFile(GameManager::GetImagePath("box.png"));

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(position.x / SCALE, position.y / SCALE);
    BodyDef.type = b2_dynamicBody;

    Body = world->CreateBody(&BodyDef);

    SetRotaiton(rotation);

    // Creates a box shape. Divide your desired width and height by 2.
    b2PolygonShape Shape;

    Shape.SetAsBox(SpriteOrigin.x / SCALE, SpriteOrigin.y / SCALE);
    b2FixtureDef FixtureDef;
    FixtureDef.density = density;
    FixtureDef.friction = friction;
    FixtureDef.shape = &Shape;
    Body->CreateFixture(&FixtureDef);
}

Player::~Player() {}

void Player::Init() {}

void Player::Input()
{
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
}

void Player::Update()
{
    Object::Update();
}
