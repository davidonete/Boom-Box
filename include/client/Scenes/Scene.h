//
//  Scene.h
//  Multiplayer Game
//
//  Created by David Parra on 08/12/16.
//  Copyright � 2016 David Parra. All rights reserved.
//

#ifndef SCENE_H
#define SCENE_H

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFGUI/Renderers.hpp> 
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "Game/Platform.h"
#include "Game/Player.h"

enum GameScene
{
	GameScene_LogIn,
	GameScene_WaitRoom,
	GameScene_Battle
};

struct ObjectsToDestroy
{
    unsigned int index;
    Object* object;
};

/** Scene.
 *  The parent scene class that controls the main loop of the objects of the scene.
 */
class Scene
{
public:
	Scene();
	virtual ~Scene();
    
    /** Initialize the scene. */
    virtual void Init();
    
    /** Handle the input for all the objects of the scene. */
	virtual void Input();
    
    /** Updates the scene and all the objects. */
	virtual void Update();
    
    /** Renders the scene. */
	virtual void Render();
    
    /** Gets a reference of the world instance. */
	inline b2World* GetWorld() { return World; }

protected:
    /** Creates a player instance on the scene.
     *  @param position Initial position of the player.
     *  @param rotation Initial rotation.
     *  @param density The weight of the player.
     *  @param friction The friction resistance of the player.
     *  @param world The reference to the scene world.
     *  @param ID The player ID.
     *  @param hasBomb If the player starts with the bomb.
     */
    void AddPlayer(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world, unsigned int ID, bool hasBomb);
    
    /** Creates a platform instance on the scene.
     *  @param position Initial position of the platform.
     *  @param rotation Initial rotation.
     *  @param density The weight of the platform.
     *  @param friction The friction resistance of the platform.
     *  @param world The reference to the scene world.
     */
    void AddPlatform(Vec2 position, float32 rotation, float32 density, float32 friction, b2World* world);
    
    /** Creates an object instance on the scene.
     *  @param position Initial position of the object.
     *  @param rotation Initial rotation.
     *  @param density The weight of the object.
     *  @param friction The friction resistance of the object.
     *  @param texturePath The path of the texture.
     *  @param world The reference to the scene world.
     */
    void AddObject(Vec2 position, Vec2 scale, float rotation, BodyType type, float32 density, float32 friction, const char* texturePath, b2World* world);
    
    /** Destroys the specific player.
     *  @param ID The player ID.
     */
    void DestroyPlayer(unsigned int ID);

	std::vector<std::unique_ptr<Object>> Objects;
    std::vector<ObjectsToDestroy> objectsToDestroy;

	b2World* World = nullptr;

    bool windowFocus = true;

private:

};

#endif
