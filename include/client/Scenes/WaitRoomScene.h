//
//  WaitRoomScene.h
//  Multiplayer Game
//
//  Created by David Parra on 04/01/17.
//  Copyright � 2016 David Parra. All rights reserved.
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

/** Wait room scene.
 *  The scene with the chat and the player information.
 */
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
  /** Client request all the players info to update the room data. */
  void UpdateRoomInfo();
  /** Updates the GUI given the information received (UpdateRoomInfo). */
  void UpdateGUI();
    
  /** Adds the player info to the GUI.
   *  @param username The player username.
   *  @param wincount The player amount of games won.
   *  @param me If the player is the current user.
   *  @param row The row.
   */
  void AddPlayerInfo(const char* username, unsigned int wincount, bool me, unsigned int row);
    
  /** Initialize the GUI. */
  void InitGUI();
  /** Function called when the player presses the chat send button. */
  void OnSendPressed();
  /** Function called when the player presses the log out button or closes the window. */
  void OnLogOutPressed();
  /** Function called when the player presses the start game button. */
  void OnStartGamePressed();

  /** Function called by thread that gets the TCP packets sended by the server. */
  void GetServerPackets();
    
  /** Prints the chat message on the GUI.
   *  @param message The message.
   */
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
