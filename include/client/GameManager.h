#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Scenes/Scene.h"

enum InputData
{
  InputData_LeftPressed,
  InputData_RightPressed,
  InputData_SpacePressed
};

class GameManager
{
public:
  ~GameManager();

  static GameManager* GetInstance();

  void Init();
  void Input();
  void Update();
  void Render();

  inline sf::RenderWindow* GetWindow() { return Window; }

  bool CheckInputPressed(InputData InputType);
  void ChangeScene(GameScene scene);

  void CloseClient();

private:
  GameManager();
  void UpdateInput();

  sf::RenderWindow* Window;
  
  std::vector<InputData> LastInputPressed;
  Scene* SceneInstance;

  static GameManager* Instance;
};

#endif
