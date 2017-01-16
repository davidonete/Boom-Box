//
//  WaitRoomScene.h
//  Multiplayer Game
//
//  Created by David Parra on 04/01/17.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef WAITROOMSCENE_H
#define WAITROOMSCENE_H

#include "Scenes/Scene.h"

struct PlayerGUIInfo
{
    std::string username;
    unsigned int wincount;
    bool me;
};

class WaitRoomScene : public Scene
{
public:
  WaitRoomScene();
  ~WaitRoomScene();

  void Init() override;
  void Input() override;
  void Update() override;
  void Render() override;

private:
  void UpdateRoomInfo();
  void UpdateGUI();
  void AddPlayerInfo(const char* username, unsigned int wincount, bool me, unsigned int row);

  void InitGUI();
  void OnSendPressed();
  void OnLogOutPressed();
  void OnStartGamePressed();

  void GetServerPackets();
  void PrintMessage(std::string message);

  sfg::SFGUI GUI;
  sfg::Desktop Desktop;

  sfg::Entry::Ptr TextBox;
  sfg::Box::Ptr ChatWindowMessages;
  sfg::Box::Ptr ChatWindowBox;
  sfg::Table::Ptr UsersWindowBox;
  sfg::Label::Ptr UserText;
  sfg::Window::Ptr UserWindow;
  sfg::Window::Ptr ChatWindow;

  sfg::Label::Ptr UserServerMessage;
  sfg::Table::Ptr UserButtonTable;

  sf::RenderWindow* RenderWindow;
  sf::Clock GUIClock;

  sf::Texture background;
  sf::Sprite sprite;

  bool deleteSceneRequest = false;
  bool changeNextSceneRequest = false;
  bool changePreviousSceneRequest = false;

  bool updateGUIRequest = false;
  std::vector<PlayerGUIInfo> playerGUIInfo;

  std::vector<sfg::Label::Ptr> Messages;

  sf::Mutex mutex;
  sf::Thread* serverReceiveThread;

  //GUI Configuration
  //Chat position % (0 to 1)
  float ChatPosX = 0.5f;
  float ChatPosY = 0.9f;
  //Chat size % (0 to 1)
  float ChatSizeX = 1.0f;
  float ChatSizeY = 1.0f;
  //Max chat messages showed
  int MaxChatMessages = 10;

  //Chat position % (0 to 1)
  float UserPosX = 0.5f;
  float UserPosY = 0.15f;
  //User window size % (0 to 1)
  float UserSizeX = 1.0f;
  float UserSizeY = 1.0f;
};

#endif