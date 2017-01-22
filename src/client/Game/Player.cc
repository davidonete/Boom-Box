#include "Game/Player.h"
#include "System/GameManager.h"

Player::Player(Vec2 position, float32 rotation, float32 density, float32 friction, b2World * world, unsigned int ID, bool bombStart)
    : Object(position, Vec2(55.0f, 55.0f), rotation, DynamicBody, density, friction, "player.png", world)
{
    playerID = ID;
    isPlayer = (GameManager::GetInstance()->Network->GetClientID() == ID);

    bomb.texture.loadFromFile(GameManager::GetImagePath("bomb.png"));
    bomb.spriteOrigin = Vec2(30.0f / 2.0f, 40.0f / 2.0f);

    if (isPlayer)
    {
        playerMark.texture.loadFromFile(GameManager::GetImagePath("Mark.png"));
        playerMark.spriteOrigin = Vec2(20.0f / 2.0f, 110.0f / 2.0f);
    }

    SetCollisionEnabled(true);
    jumping = false;
    hasBomb = bombStart;

    SetType(Type_Player);

    lastPacketSent.ID = 0;
    lastPacketSent.rotation = 0.0f;
    lastPacketSent.x = 0.0f;
    lastPacketSent.y = 0.0f;
}

Player::~Player()
{
    std::cout << "Destroyed child";
}

void Player::Init() {}

void Player::Input()
{
    if (isPlayer)
    {
        //Force
        if (GM->CheckInputPressed(InputData_RightPressed))
            ApplyForce(Vec2(speed, 0.0f));

        if (GM->CheckInputPressed(InputData_LeftPressed))
            ApplyForce(Vec2(-speed, 0.0f));

        if (GM->CheckInputPressed(InputData_SpacePressed))
        {
            if (!jumping)
            {
                jumping = true;
                ApplyImpulse(Vec2(0.0, -jumpForce));
            }
        }
    }
}

void Player::Update()
{
    Object::Update();

    if(hasBomb)
        UpdateSprite(&bomb, GetPosition(), 0.0f);
    if (isPlayer)
        UpdateSprite(&playerMark, GetPosition(), 0.0f);

    UpdatePlayerServer();
}

void Player::Render()
{
    Object::Render();

    if (hasBomb && isPlayer)
        RenderSprite(&bomb);
    if (isPlayer)
        RenderSprite(&playerMark);
}

void Player::SetBomb(bool bomb)
{
    if (bomb)
    {
        speed = speed * 1.3f;
        hasBomb = bomb;
    }
    else
    { 
        speed = speed / 1.3f;
        hasBomb = bomb;
    }
}

void Player::OnCollisionDetected(Object* otherObject)
{
    Object::OnCollisionDetected(otherObject);
    if (otherObject->GetType() == Type_Ground && jumping)
        jumping = false;
    else if (otherObject->GetType() == Type_Player)
    {
        if (GM->Network->IsAuthority())
        {
            Player* otherPlayer = static_cast<Player*>(otherObject);
            if (HaveBomb())
            {
                PlayerBombChangePacket packet;
                packet.ID = GetPlayerID();
                packet.otherID = otherPlayer->GetPlayerID();
                packet.msg = NetworkManager::GetCodeFromRequest(Request_PlayerChangeBomb);
                GM->Network->SendPacket(packet);
            }
        }
    }
}

void Player::UpdatePlayerServer()
{
    if (isPlayer)
    {
        Vec2 pos = GetPosition();
        float rot = GetRotation();
        float offset = 1.0f;
        if (!(pos.x > (lastPacketSent.x - offset) && pos.x < (lastPacketSent.x + offset)) || !(pos.y >(lastPacketSent.y - offset) && pos.y < (lastPacketSent.y + offset)) || !(rot >(lastPacketSent.rotation - offset) && rot < (lastPacketSent.rotation + offset)))
        {
            GamePacket packet;
            packet.ID = GM->Network->GetClientID();
            packet.rotation = rot;
            packet.x = pos.x;
            packet.y = pos.y;
            GM->Network->SendPacket(packet);
            lastPacketSent = packet;
        }
    }
}
