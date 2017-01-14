#include "Game/Object.h"
#include "System/GameManager.h"

Object::Object()
{
    GM = GameManager::GetInstance();
    Window = GM->GetWindow();

    collisionManager = new CollisionManager();
}

Object::Object(Vec2 position, Vec2 scale, float32 rotation, ObjectType type, float32 density, float32 friction, const char* texturePath, b2World* world)
{
    GM = GameManager::GetInstance();
    Window = GM->GetWindow();

    b2BodyDef BodyDef;
    BodyDef.position = b2Vec2(position.x / SCALE, position.y / SCALE);

    if (type == StaticBody)
        BodyDef.type = b2_staticBody;
    else if (type == DynamicBody)
        BodyDef.type = b2_dynamicBody;

    object.body = world->CreateBody(&BodyDef);

    SetRotaiton(rotation);

    // Creates a box shape. Divide your desired width and height by 2.
    b2PolygonShape Shape;

    Shape.SetAsBox((scale.x / 2.0f) / SCALE, (scale.y / 2.0f) / SCALE);
    b2FixtureDef FixtureDef;
    FixtureDef.density = density;
    FixtureDef.friction = friction;
    FixtureDef.shape = &Shape;
    object.body->CreateFixture(&FixtureDef);

    object.sprite.texture.loadFromFile(GameManager::GetImagePath(texturePath));
    object.sprite.spriteOrigin = Vec2(scale.x/2.0f, scale.y/2.0f);

    collisionManager = new CollisionManager();
    object.body->SetUserData(this);
    world->SetContactListener(collisionManager);
}

Object::~Object() 
{
    delete collisionManager;
}

void Object::Init() {}

void Object::Input() {}

void Object::Update()
{
    UpdateSprite(&object.sprite, GetPosition(), GetRotation());
}

void Object::Render()
{
    RenderSprite(&object.sprite);
}

Vec2 Object::GetPosition()
{
    return Vec2(object.body->GetPosition().x * SCALE, object.body->GetPosition().y * SCALE);
}

float32 Object::GetRotation()
{
    float32 result;
    if (object.body->GetType() == b2_staticBody)
        result = (180 / b2_pi *  object.body->GetAngle());
    else if (object.body->GetType() == b2_dynamicBody)
        result = (object.body->GetAngle() * 180 / b2_pi);

    return result;
}

void Object::SetPosition(Vec2 pos)
{
    object.body->SetTransform(b2Vec2(pos.x / SCALE, pos.y / SCALE), object.body->GetAngle());
}

void Object::OnCollisionDetected(Object * otherObject)
{

}

void Object::SetRotaiton(float32 angle)
{
    object.body->SetTransform(object.body->GetPosition(), angle / (180 / b2_pi));
}

void Object::ApplyForce(Vec2 force)
{
    if (object.body->GetType() != b2_staticBody)
    {
        b2Vec2 vel = object.body->GetLinearVelocity();
        float32 fx = 0.0f;
        float32 fy = 0.0f;
        if (force.x != 0.0f)
        {
            //f = mv/t
            float velXChange = force.x - vel.x;
            fx = object.body->GetMass() * velXChange / (1.0f / 60.0f);
        }
        if (force.y != 0.0f)
        {
            float velYChange = force.y - vel.y;
            float32 fy = object.body->GetMass() * velYChange / (1.0f / 60.0f);
        }
        object.body->ApplyForce(b2Vec2(fx, fy), object.body->GetWorldCenter(), true);
    }
}

void Object::ApplyImpulse(Vec2 impulse)
{
    if (object.body->GetType() != b2_staticBody)
    {
        b2Vec2 vel = object.body->GetLinearVelocity();
        float32 ix = 0.0f;
        float32 iy = 0.0f;
        if (impulse.x != 0.0f)
        {
            float velXChange = impulse.x - vel.x;
            ix = object.body->GetMass() * velXChange;
        }
        if (impulse.y != 0.0f)
        {
            float velYChange = impulse.y - vel.y;
            iy = object.body->GetMass() * velYChange;
        }
        object.body->ApplyLinearImpulse(b2Vec2(ix, iy), object.body->GetWorldCenter(), true);
    }
}

void Object::UpdateSprite(ObjectSprite* sprite, Vec2 pos, float32 rot)
{
    sprite->position = pos;
    sprite->rotation = rot;

    sf::Sprite updatedSprite;
    updatedSprite.setTexture(sprite->texture);

    updatedSprite.setOrigin(sprite->spriteOrigin.x, sprite->spriteOrigin.y);
    updatedSprite.setPosition(sprite->position.x, sprite->position.y);
    updatedSprite.setRotation(sprite->rotation);

    sprite->sprite = updatedSprite;
}

void Object::RenderSprite(ObjectSprite* sprite)
{
    Window->draw(sprite->sprite);
}