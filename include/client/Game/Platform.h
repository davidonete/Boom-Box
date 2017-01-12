//
//  Platform.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Object.h"

class Platform : public Object
{
public:
  Platform(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world);
  ~Platform();

  void Init() override;
  void Input() override;
  void Update() override;

private:

};

#endif