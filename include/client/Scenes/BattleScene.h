//
//  BattleScene.h
//  Multiplayer Game
//
//  Created by David Parra on 04/01/17.
//  Copyright � 2016 David Parra. All rights reserved.
//

#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include "Scenes/Scene.h"

class BattleScene : public Scene
{
public:
  BattleScene();
  ~BattleScene();

private:
  void Init() override;
  void Input() override;
  void Update() override;
  void Render() override;

  void GetServerTCPPackets();
  void GetServerUDPPackets();

  void UpdatePlayers();
  void UpdatePlayerDelete();
  void UpdateTimer();

  sf::RenderWindow* RenderWindow;

  sf::Texture background;
  sf::Sprite sprite;

  float timeLeft = 20.0f;
  sf::Text timeText;
  sf::Font myfont;
  sf::Clock clock;

  bool changeSceneRequest = false;
  bool deleteSceneRequest = false;

  sf::Mutex mutex;
  sf::Thread* serverReceiveTCPThread;
  sf::Thread* serverReceiveUDPThread;

  std::vector<GamePacket> lastGamePacketsReceived;
  std::vector<unsigned int> lastDeadPlayerReceived;
};

#endif
