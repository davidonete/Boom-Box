//
//  BattleScene.h
//  Multiplayer Game
//
//  Created by David Parra on 04/01/17.
//  Copyright © 2016 David Parra. All rights reserved.
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

  sf::RenderWindow* RenderWindow;

  bool changeSceneRequest = false;
  bool deleteSceneRequest = false;
};

#endif