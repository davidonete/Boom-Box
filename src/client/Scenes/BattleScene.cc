#include "Scenes/BattleScene.h"
#include "System/GameManager.h"

BattleScene::BattleScene() {}

BattleScene::~BattleScene() {}

void BattleScene::Init()
{
    RenderWindow = GameManager::GetInstance()->GetWindow();

    //Initialize the scene world with gravity values
    World = new b2World(b2Vec2(0.0f, 9.8f));

    //Create map
    AddPlatform(Vec2(585.0f, 150.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlatform(Vec2(215.0f, 250.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlatform(Vec2(585.0f, 350.0f), 0.0f, 0.0f, 1.0f, World);
    AddPlatform(Vec2(215.0f, 445.0f), 0.0f, 0.0f, 1.0f, World);
    AddObject(Vec2(425.0f, 600.0f), Vec2(850.0f, 95.0f), 0.0f, StaticBody, 0.0f, 1.0f, "ground.png", World);

    std::vector<Vec2> startPosition;
    startPosition.push_back(Vec2(200.0f, 550.0f));
    startPosition.push_back(Vec2(215.0f, 200.0f));
    startPosition.push_back(Vec2(585.0f, 100.0f));
    startPosition.push_back(Vec2(600.0f, 550.0f));

    background.loadFromFile(GameManager::GetImagePath("background.png"));
    sprite.setTexture(background);

    myfont.loadFromFile(GameManager::GetFontPath("arial.ttf"));

    timeText.setFont(myfont);
    timeText.setColor(sf::Color(145, 145, 145));
    timeText.setScale(1.f, 1.f);
    timeText.move(730.f, 10.f);

    NetworkManager* Network = GameManager::GetInstance()->Network;

    ClientRequestPacket request;
    request.ID = Network->GetClientID();
    request.msg = Network->GetCodeFromRequest(Request_GetPlayersInfo);
    Network->SendPacket(request);

    const int packetSize = sizeof(PlayerInfoPacket);
    bool allInfoReceived = false;
    int numPlayers = 0;
    while (!allInfoReceived)
    {
        char buffer[256];
        memset(buffer, -52, 256);
        Network->ReceivePacket(TCP, buffer);

        //If receiving multiple packets at once
        if (Network->GetSizeOfBytes(buffer) > packetSize)
        {
            int numberOfPackets = Network->GetSizeOfBytes(buffer) / packetSize;
            for (int i = 0; i < numberOfPackets; i++)
            {
                char bufferPart[packetSize];
                memcpy(bufferPart, buffer + (packetSize * i), packetSize);

                PlayerInfoPacket player;
                Network->GetPacketFromBytes(bufferPart, player);
                if (Network->GetPacketType(bufferPart) == Type_PlayerInfoPacket)
                {
                    if (player.ID == Network->GetClientID())
                        Network->SetAuthority(player.authority);

                    AddPlayer(startPosition[player.ID -1], 0.0f, 1.0f, 1.2f, World, player.ID, (bool)player.wincount);
                    numPlayers++;
                }
            }
            allInfoReceived = true;
        }
        else
        {
            if (Network->GetPacketType(buffer) == Type_PlayerInfoPacket)
            {
                PlayerInfoPacket player;
                Network->GetPacketFromBytes(buffer, player);
                if (player.ID == Network->GetClientID())
                {
                    Network->SetAuthority(player.authority);
                    allInfoReceived = true;
                }
                AddPlayer(startPosition[player.ID -1], 0.0f, 1.0f, 1.2f, World, player.ID, (bool)player.wincount);
                numPlayers++;
            }
        }
    }

    ClientRequestPacket readyRequest;
    readyRequest.ID = Network->GetClientID();
    readyRequest.msg = Network->GetCodeFromRequest(Request_PlayerReady);
    Network->SendPacket(readyRequest);

    bool startGame = false;
    while (!startGame)
    {
        char buffer1[256];
        memset(buffer1, -52, 256);
        Network->ReceivePacket(TCP, buffer1);

        PacketType packetType = Network->GetPacketType(buffer1);
        if (packetType == Type_ServerMessagePacket)
        {
            ServerMessagePacket packet;
            Network->GetPacketFromBytes(buffer1, packet);
            ServerMessage msg = Network->GetServerMessage(packet.msg);
            if (msg == Server_StartBattle)
                startGame = true;
        }
    }

    serverReceiveTCPThread = new sf::Thread(std::bind(&BattleScene::GetServerTCPPackets, this));
    serverReceiveTCPThread->launch();

    Network->Connect(UDP);

    serverReceiveUDPThread = new sf::Thread(std::bind(&BattleScene::GetServerUDPPackets, this));
    serverReceiveUDPThread->launch();
}

void BattleScene::Input()
{
    Scene::Input();
}

void BattleScene::Update()
{
    sf::Event event;
    while (RenderWindow->pollEvent(event))
    {
        // Close window: exit
        if (event.type == sf::Event::Closed)
            deleteSceneRequest = true;
        else if (event.type == sf::Event::GainedFocus)
            windowFocus = true;
        else if (event.type == sf::Event::LostFocus)
            windowFocus = false;
    }

    for (int i = 0; i < Objects.size(); i++)
    {
        if (Objects[i]->GetType() == Type_Player)
        {
            Vec2 pos = Objects[i]->GetPosition();
            if (pos.x > 831.0f)
                Objects[i]->SetPosition(Vec2(-30.0f, pos.y));
            else if (pos.x < -31.0f)
                Objects[i]->SetPosition(Vec2(830.0f, pos.y));
            if (pos.y > 530.0f)
                Objects[i]->SetPosition(Vec2(pos.x, 519.0f));
        }
    }

    UpdatePlayers();
    UpdateTimer();

    Scene::Update();
}

void BattleScene::Render()
{
    RenderWindow->draw(sprite);
    RenderWindow->draw(timeText);
    Scene::Render();

    if (deleteSceneRequest || changeSceneRequest)
    {
        if (deleteSceneRequest)
            GameManager::GetInstance()->CloseGame();
        else if (changeSceneRequest)
        {
            GameManager::GetInstance()->Network->Disconnect(UDP);
            GameManager::GetInstance()->ChangeScene(GameScene_WaitRoom);
        }
    }
}

void BattleScene::GetServerTCPPackets()
{
    NetworkManager* Network = GameManager::GetInstance()->Network;
    while (!deleteSceneRequest && !changeSceneRequest)
    {
        char buffer[256];
        memset(buffer, -52, 256);
        Network->ReceivePacket(TCP, buffer);

        PacketType packetType = Network->GetPacketType(buffer);
        if (packetType == Type_ServerMessagePacket)
        {
            ServerMessagePacket packet;
            Network->GetPacketFromBytes(buffer, packet);
            ServerMessage msg = Network->GetServerMessage(packet.msg);
            if (msg == Server_PlayerDisconnected)
                changeSceneRequest = true;
        }
        else if (packetType == Type_ServerChangeBombPacket)
        {
            ServerChangeBombPacket packet;
            Network->GetPacketFromBytes(buffer, packet);

            for (int i = 0; i < Objects.size(); i++)
            {
                if (Objects[i]->GetType() == Type_Player)
                {
                    Player *player = dynamic_cast<Player*>(Objects[i].get());
                    if (player->GetPlayerID() == packet.fromID)
                        player->SetBomb(false);
                    else if (player->GetPlayerID() == packet.toID)
                        player->SetBomb(true);
                }
            }
        }
        else if (packetType == Type_ClientRequestPacket)
        {
            ClientRequestPacket packet;
            Network->GetPacketFromBytes(buffer, packet);
            ServerMessage msg = Network->GetServerMessage(packet.msg);
            if (msg == Server_PlayerDead)
            {
                mutex.lock();
                DestroyPlayer(packet.ID);
                mutex.unlock();
            }
        }

        sf::sleep(sf::milliseconds(16));
    }
}

void BattleScene::GetServerUDPPackets()
{
    NetworkManager* Network = GameManager::GetInstance()->Network;
    while (!deleteSceneRequest && !changeSceneRequest)
    {
        char buffer[256];
        memset(buffer, -52, 256);
        Network->ReceivePacket(UDP, buffer);

        PacketType packetType = Network->GetPacketType(buffer);
        if (packetType == Type_GamePacket)
        {
            GamePacket packet;
            Network->GetPacketFromBytes(buffer, packet);
            if (packet.ID != Network->GetClientID())
            {
                mutex.lock();
                lastGamePacketsReceived.push_back(packet);
                mutex.unlock();
            }
        }
    }
}

void BattleScene::UpdatePlayers()
{
    mutex.lock();

    while (lastGamePacketsReceived.size() > 0)
    {
        GamePacket packet = lastGamePacketsReceived[0];
        for (int i = 0; i < Objects.size(); i++)
        {
            if (Objects[i]->GetType() == Type_Player)
            {
                Player *player = dynamic_cast<Player*>(Objects[i].get());
                if (player->GetPlayerID() == packet.ID)
                {
                    player->SetPosition(Vec2(packet.x, packet.y));
                    player->SetRotaiton(packet.rotation);
                    break;
                }
            }
        }
        lastGamePacketsReceived.erase(lastGamePacketsReceived.begin());
    }
    mutex.unlock();
}

void BattleScene::UpdateTimer()
{
    if (timeLeft > 0)
    {
        auto seconds = clock.getElapsedTime().asSeconds();
        // Only update every 1 second
        if (seconds > 1)
        {
            timeLeft--;
            std::ostringstream ss;
            ss << timeLeft;
            timeText.setString(ss.str());
            clock.restart();

            if (timeLeft == 0)
            { 
                NetworkManager* Network = GameManager::GetInstance()->Network;
                if (Network->IsAuthority())
                {
                    ClientRequestPacket packet;
                    packet.ID = Network->GetClientID();
                    packet.msg = Network->GetCodeFromRequest(Request_TimeOver);
                    Network->SendPacket(packet);
                }
            }
        }
    }
}
