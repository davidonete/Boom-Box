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

/** Battle scene.
 *  The scene of the game that has the battle between players.
 */
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

  /** Function called by thread that gets the TCP packets sended by the server. */
  void GetServerTCPPackets();
  /** Function called by thread that gets the UDP packets sended by the server. */
  void GetServerUDPPackets();
    
  /** Updates players using the UDP packets received. */
  void UpdatePlayers();
  /** Updates players death using the TCP packets received. */
  void UpdatePlayerDelete();
  /** Updates the game timer. */
  void UpdateTimer();

  sf::RenderWindow* RenderWindow;

  sf::Texture background;
  sf::Sprite sprite;

  float timeLeft = 120.0f;
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
