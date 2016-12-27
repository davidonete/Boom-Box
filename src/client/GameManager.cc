#include "GameManager.h"
#include "Scenes/LoginScene.h"

GameManager* GameManager::Instance = nullptr;

GameManager::GameManager() {}

GameManager::~GameManager() 
{
    delete SceneInstance;
    LastInputPressed.clear();
}

GameManager* GameManager::GetInstance()
{
    if(Instance == nullptr)
		Instance = new GameManager();

    return Instance;
}

void GameManager::Init()
{
    //Initialize window (width, height, bits per pixel)
    Window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "SFML window");
    Window->setFramerateLimit(60);

    SceneInstance = nullptr;

  //Initialize world with gravity value
  //world = new b2World(b2Vec2(0.0f, 9.8f));
}

void GameManager::Input()
{
	UpdateInput();
	SceneInstance->Input();
}

void GameManager::Update()
{
	SceneInstance->Update();
}

void GameManager::Render()
{
    Window->clear(sf::Color::White);

    SceneInstance->Render();

    //Update the window rendering
    Window->display();
}

void GameManager::UpdateInput()
{
    LastInputPressed.clear();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        LastInputPressed.push_back(InputData_RightPressed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        LastInputPressed.push_back(InputData_LeftPressed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        LastInputPressed.push_back(InputData_SpacePressed);
}

bool GameManager::CheckInputPressed(InputData InputType)
{
    for(unsigned int i = 0; i < LastInputPressed.size(); i++)
    {
        if(LastInputPressed[i] == InputType)
            return true;
    }
    return false;
}

void GameManager::ChangeScene(GameScene scene)
{
    if (SceneInstance != nullptr)
        delete SceneInstance;

    switch (scene)
    {
        case GameScene_LogIn:
            SceneInstance = new LoginScene();
        break;

        default:
            SceneInstance = new Scene();
        break;
    }

    SceneInstance->Init();
}
