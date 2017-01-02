//
//  Scene.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright © 2016 David Parra. All rights reserved.
//

#ifndef SCENE_H
#define SCENE_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFGUI/Renderers.hpp> 
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "Game/Object.h"

enum GameScene
{
	GameScene_LogIn,
	GameScene_WaitRoom,
	GameScene_Battle
};

class Scene
{
public:
	Scene();
	virtual ~Scene();

    virtual void Init();
	virtual void Input();
	virtual void Update();
	virtual void Render();

	inline b2World* GetWorld() { return World; }
	void AddObject(Vec2 position, Vec2 scale, ObjectType type, float32 density, float32 friction, const char* texturePath, Vec2 spriteOrigin);

	//GameScene SceneType;

protected:
	std::vector<Object> Objects;
	b2World* World;

private:

};

#endif