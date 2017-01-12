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
  InputData_SpacePressed,
  InputData_EnterPressed,
  InputData_TabPressed
};

/** Game Manager.
 *  Has the control of the main loop, the scenes and the networking of the game.
 */
class GameManager
{
public:
  ~GameManager();

  /** Creates or returns the instance of the singleton.
   *  @return A pointer to the Game Manager instance.
   */
  static GameManager* GetInstance();

  /** Handles the input detected and send it to the active scene. */
  void Input();
  /** Called each frame to update the active scene. */
  void Update();
  /** Called each frame to render the active scene. */
  void Render();

  /** Get the current used window of the game.
   *  @return Reference to the current used window.
   */
  inline sf::RenderWindow* GetWindow() { return Window; }

  /** Get the path of a file depending on the machine OS.
   *  @param filename The name of the file.
   *  @return Relative path to the image file.
   */
  static std::string GetImagePath(const char * filename);

  /** Get if a specific input has been pressed recently.
   *  @param InputType The input type to check.
   */
  bool CheckInputPressed(InputData InputType);

  /** Changes the displayed scene for a specific one.
   *  @param scene The new scene to change.
   */
  void ChangeScene(GameScene scene);

  /** Requests the game to close, closing the window and deleting all dependant modules.*/
  void CloseGame();

  // A reference to the networking manager class.
  NetworkManager* Network;

private:
  GameManager();

  /** Initializes all the modules needed and the first scene of the game. */
  void Init();
  void UpdateInput();

  Scene* SceneInstance;
  sf::RenderWindow* Window;
  std::vector<InputData> LastInputPressed;

  static GameManager* Instance;
};

#endif
