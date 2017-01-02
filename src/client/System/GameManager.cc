#include "System/GameManager.h"
#include "Scenes/LoginScene.h"

//Path of assets on the different OS targets
#ifdef _WIN32
    #define IMAGE_PATH "../../../data/images/"
#elif __APPLE__
    #define IMAGE_PATH "data/images/"
#endif

GameManager* GameManager::Instance = nullptr;

GameManager::GameManager() {}

GameManager::~GameManager() 
{
    delete Network;
    delete Window;
    delete SceneInstance;
    LastInputPressed.clear();
}

GameManager* GameManager::GetInstance()
{
    if (Instance == nullptr)
    {
        Instance = new GameManager();
        Instance->Init();
    }

    return Instance;
}

void GameManager::Init()
{
    //Initialize window (width, height, bits per pixel)
    Window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), "Super Awesome Game - v1.0");//, sf::Style::Fullscreen);
    Window->setFramerateLimit(60);

    Network = new NetworkManager();
    Network->Init();

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
    {
        delete SceneInstance;
        SceneInstance = nullptr;
    }

    switch (scene)
    {
        case GameScene_LogIn:
            SceneInstance = new LoginScene();
        break;

        case GameScene_WaitRoom:
            SceneInstance = new Scene();
        break;

        default:
            SceneInstance = new Scene();
        break;
    }

    SceneInstance->Init();
}

void GameManager::CloseGame()
{
    Network->Disconnect(TCP);
    Network->Disconnect(UDP);
    Window->close();
}

std::string GameManager::GetImagePath(const char* filename) {
    std::string path = IMAGE_PATH;
    return path.append(filename);
}
