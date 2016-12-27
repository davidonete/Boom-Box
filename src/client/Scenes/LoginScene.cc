#include "Scenes/LoginScene.h"
#include "GameManager.h"

LoginScene::LoginScene() {}

LoginScene::~LoginScene() {}

void LoginScene::Init() 
{
    RenderWindow = GameManager::GetInstance()->GetWindow();
    //RenderWindow->resetGLStates();

    //Window = sfg::Window::Create();
    //Window->SetTitle("Something");
}

void LoginScene::Input() {}

void LoginScene::Update() 
{
    //Window->Update(0.0f);
}

void LoginScene::Render() 
{
    //GUI.Display(*RenderWindow);
}
