//
//  GameManager.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Scenes/Scene.h"
#include "System/NetworkManager.h"

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
  static std::string GetImagePath(const char * image);
  static std::string GetSoundPath(const char * sound);

  bool CheckInputPressed(InputData InputType);
  void ChangeScene(GameScene scene);
  void CloseGame();

  NetworkManager* Network;

private:
  GameManager();
  void UpdateInput();

  Scene* SceneInstance;
  sf::RenderWindow* Window;
  std::vector<InputData> LastInputPressed;

  static GameManager* Instance;
};

#endif
