#include "Scenes/BattleScene.h"
#include "System/GameManager.h"

BattleScene::BattleScene() {}

BattleScene::~BattleScene() {}

void BattleScene::Init()
{
    RenderWindow = GameManager::GetInstance()->GetWindow();

    //Initialize the scene world with gravity values
    World = new b2World(b2Vec2(0.0f, 9.8f));

    sf::Vector2u winSize = RenderWindow->getSize();

    AddPlatform(Vec2(585.0f, 150.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlatform(Vec2(215.0f, 250.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlatform(Vec2(585.0f, 350.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlatform(Vec2(215.0f, 445.0f), 0.0f, 0.0f, 1.0f, World);

    AddPlayer(Vec2(400.0f, 100.0f), 0.0f, 1.0f, 1.2f, World, true);
    AddPlayer(Vec2(400.0f, 100.0f), 0.0f, 1.0f, 1.2f, World, false);

    AddObject(Vec2(425.0f, 600.0f), Vec2(850.0f, 95.0f), 0.0f, StaticBody, 0.0f, 1.0f, "ground.png", World);
}

void BattleScene::Input()
{
    Scene::Input();
}

void BattleScene::Update()
{
    sf::Event event;
    while (RenderWindow->pollEvent(event))
    {
        // Close window: exit
        if (event.type == sf::Event::Closed)
            deleteSceneRequest = true;
    }

    for (int i = 0; i < players.size(); i++)
    {
        Vec2 pos = players[i]->GetPosition();
        if (pos.x > 831.0f)
            players[i]->SetPosition(Vec2(-30.0f, pos.y));
        else if (pos.x < -31.0f)
            players[i]->SetPosition(Vec2(830.0f, pos.y));
        if (pos.y > 530.0f)
            players[i]->SetPosition(Vec2(pos.x, 519.0f));
    }

    Scene::Update();
}

void BattleScene::Render()
{
    Scene::Render();

    if (deleteSceneRequest || changeSceneRequest)
    {
    if (deleteSceneRequest)
        GameManager::GetInstance()->CloseGame();
    else if (changeSceneRequest)
        GameManager::GetInstance()->ChangeScene(GameScene_WaitRoom);
    }
}
