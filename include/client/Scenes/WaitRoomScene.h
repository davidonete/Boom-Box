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
    void InitGUI();
    void OnSendPressed();
    void GetServerPackets();
    void PrintMessage(std::string message);

    sfg::SFGUI GUI;
    sfg::Desktop Desktop;

    sfg::Entry::Ptr TextBox;
    sfg::Box::Ptr ScrolledWindowBox;

    sf::RenderWindow* RenderWindow;
    sf::Clock GUIClock;

    sf::Texture background;
    sf::Sprite sprite;

    bool deleteSceneRequest = false;
    bool changeSceneRequest = false;
    std::vector<sfg::Label::Ptr> Messages;

    sf::Mutex mutex;
    sf::Thread* serverReceiveThread;

    //GUI Configuration
    //Chat position % (0 to 1)
    float ChatPosX = 0.5f;
    float ChatPosY = 0.5f;
    //Chat size % (0 to 1)
    float ChatSizeX = 1.0f;
    float ChatSizeY = 1.0f;
    //Max chat messages showed
    int MaxChatMessages = 10;
};

#endif