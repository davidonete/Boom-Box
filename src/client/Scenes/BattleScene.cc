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

    AddPlatform(Vec2(400.0f, 200.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlayer(Vec2(400.0f, 100.0f), 0.0f, 0.0f, 5.0f, World, true);
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
