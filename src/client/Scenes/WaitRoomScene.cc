#include "Scenes/WaitRoomScene.h"
#include "System/GameManager.h"

WaitRoomScene::WaitRoomScene() {}

WaitRoomScene::~WaitRoomScene()
{
    serverReceiveThread->terminate();
    Desktop.RemoveAll();
}

void WaitRoomScene::Init()
{
    RenderWindow = GameManager::GetInstance()->GetWindow();

    sf::Texture::getMaximumSize();
    sf::Shader::isAvailable();

    sfg::Renderer::Set(sfg::VertexArrayRenderer::Create());

    RenderWindow->resetGLStates();

    //GUI
    InitGUI();

    UpdateRoomInfo();

    //Background
    background.loadFromFile(GameManager::GetImagePath("background.jpg"));
    sprite.setTexture(background);

    sf::Vector2u winSize = RenderWindow->getSize();
    sf::Vector2f bgScale;
    bgScale.x = (winSize.x * 0.5f) / 800.0f;
    bgScale.y = (winSize.y * 0.7f) / 600.0f;
    sprite.setScale(bgScale);

    //Receive server packets
    serverReceiveThread = new sf::Thread(std::bind(&WaitRoomScene::GetServerPackets, this));
    serverReceiveThread->launch();
}

void WaitRoomScene::InitGUI()
{
    std::shared_ptr<sf::Font> my_font = std::make_shared<sf::Font>();
    my_font->loadFromFile(GameManager::GetFontPath("arial.ttf"));
    sfg::Context::Get().GetEngine().GetResourceManager().SetDefaultFont( my_font );
    sfg::Context::Get().GetEngine().SetProperty( "*", "FontName", GameManager::GetFontPath("arial.ttf"));
    
    sf::Vector2f winSize = (sf::Vector2f)RenderWindow->getSize();
    sf::Vector2f ChatSize(((winSize.x * 700.f) / 1366.0f) * ChatSizeX, ((winSize.y * 250.f) / 768.0f) * ChatSizeY);
    MaxChatMessages = (int)((ChatSize.y * 10) / 160.0f);

    ChatWindow = sfg::Window::Create(sfg::Window::BACKGROUND | sfg::Window::SHADOW);

    auto button = sfg::Button::Create("Send");
    button->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&WaitRoomScene::OnSendPressed, this));

    ChatWindowMessages = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

    auto scrolledwindow = sfg::ScrolledWindow::Create();
    scrolledwindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC);
    scrolledwindow->AddWithViewport(ChatWindowMessages);
    scrolledwindow->SetRequisition(ChatSize);

    TextBox = sfg::Entry::Create();
    TextBox->SetMaximumLength(45);
    TextBox->SetRequisition(sf::Vector2f(250.0f, 35.0f));

    sfg::Table::Ptr Table = sfg::Table::Create();
    Table->Attach(TextBox, sf::Rect<sf::Uint32>(0, 0, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);
    Table->Attach(button, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);

    ChatWindowBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    ChatWindowBox->Pack(scrolledwindow, true, true);
    ChatWindowBox->Pack(Table, false, true);

    ChatWindow->Add(ChatWindowBox);

    sf::Vector2f loginSize = ChatWindow->GetRequisition();
    sf::Vector2f pos = sf::Vector2f((winSize.x - loginSize.x) * ChatPosX, (winSize.y - loginSize.y) * ChatPosY);
    ChatWindow->SetPosition(pos);

    Desktop.Add(ChatWindow);

    sf::Vector2f UserSize(((winSize.x * 550.f) / 1366.0f) * UserSizeX, ((winSize.y * 160.f) / 768.0f) * UserSizeY);

    UserWindow = sfg::Window::Create(sfg::Window::BACKGROUND | sfg::Window::SHADOW);
    UsersWindowBox = sfg::Table::Create();

    auto userscrolledwindow = sfg::ScrolledWindow::Create();
    userscrolledwindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC);
    userscrolledwindow->AddWithViewport(UsersWindowBox);
    userscrolledwindow->SetRequisition(UserSize);

    std::stringstream text;
    text << "Users in the room: 0";
    UserText = sfg::Label::Create();
    UserText->SetText(text.str());
    UserText->SetAlignment(sf::Vector2f(0.0f, 0.0f));
    
    UserButtonTable = sfg::Table::Create();
    UserButtonTable->SetRowSpacings(5.f);
    UserButtonTable->SetColumnSpacings(5.f);

    UserServerMessage = sfg::Label::Create();
    UserServerMessage->SetAlignment(sf::Vector2f(0.0f, 0.0f));

    auto box2 = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    box2->Pack(UserText);
    box2->Pack(userscrolledwindow, false, true);
    box2->Pack(UserServerMessage);
    box2->Pack(UserButtonTable, true, true);

    UserWindow->Add(box2);

    sf::Vector2f UserWindowSize = UserWindow->GetRequisition();
    sf::Vector2f userPos = sf::Vector2f((winSize.x - UserWindowSize.x) * UserPosX, (winSize.y - UserWindowSize.y) * UserPosY);
    UserWindow->SetPosition(userPos);

    Desktop.Add(UserWindow);

    Desktop.Update(0.0f);
    TextBox->GrabFocus();
}

void WaitRoomScene::Input() {}

void WaitRoomScene::Update()
{
    sf::Event event;
    while (RenderWindow->pollEvent(event))
    {
        Desktop.HandleEvent(event);
        if (event.type == sf::Event::Closed)
            deleteSceneRequest = true;
        else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
            OnSendPressed();
        else if (event.type == sf::Event::Resized)
        {
            sf::Vector2f winSize = (sf::Vector2f)RenderWindow->getSize();

            sf::Vector2f UserWindowSize = UserWindow->GetRequisition();
            sf::Vector2f userPos = sf::Vector2f((winSize.x - UserWindowSize.x) * UserPosX, (winSize.y - UserWindowSize.y) * UserPosY);
            UserWindow->SetPosition(userPos);
            
            sf::Vector2f loginSize = ChatWindow->GetRequisition();
            sf::Vector2f pos = sf::Vector2f((winSize.x - loginSize.x) * ChatPosX, (loginSize.y + userPos.y) * ChatPosY * 1.1f);
            ChatWindow->SetPosition(pos);

            MaxChatMessages = (int)((loginSize.y * 10) / 160.0f);
        }
    }

    auto microseconds = GUIClock.getElapsedTime().asMicroseconds();

    // Only update every 5ms
    if (microseconds > 5000) 
    {
        Desktop.Update(static_cast<float>(microseconds) / 1000000.f);
        GUIClock.restart();
    }
}

void WaitRoomScene::Render()
{
    try
    {
        mutex.lock();
        RenderWindow->draw(sprite);
        GUI.Display(*RenderWindow);
        mutex.unlock();
    }
    catch (int e) { e = 0; }

    if (updateGUIRequest)
        UpdateGUI();

    if (deleteSceneRequest || changeNextSceneRequest || changePreviousSceneRequest)
    {
        if (deleteSceneRequest)
            GameManager::GetInstance()->CloseGame();
        else if (changeNextSceneRequest)
            GameManager::GetInstance()->ChangeScene(GameScene_Battle);
        else if (changePreviousSceneRequest)
            GameManager::GetInstance()->ChangeScene(GameScene_LogIn);
    }
}

void WaitRoomScene::OnSendPressed() 
{
    if (!TextBox->GetText().isEmpty())
    {
        GameManager* GM = GameManager::GetInstance();

        std::string text = TextBox->GetText().toAnsiString();
        TextBox->SetText("");
        std::string username = GM->Network->GetUsername();

        std::stringstream buffer;
        buffer << "[" << username << "]: " << text;

        ChatPacket packet;
        packet.ID = GM->Network->GetClientID();
        std::strcpy(packet.message, buffer.str().c_str());
        GM->Network->SendPacket(packet);
    }
}

void WaitRoomScene::OnLogOutPressed()
{
    serverReceiveThread->terminate();
    GameManager::GetInstance()->Network->LogOut();
    GameManager::GetInstance()->Network->Disconnect(TCP);
    changePreviousSceneRequest = true;
}

void WaitRoomScene::OnStartGamePressed()
{
    NetworkManager* Network = GameManager::GetInstance()->Network;

    ClientRequestPacket packet;
    packet.ID = Network->GetClientID();
    packet.msg = Network->GetCodeFromRequest(Request_StartBattleScene);

    Network->SendPacket(packet);
}

void WaitRoomScene::GetServerPackets()
{
    NetworkManager* Network = GameManager::GetInstance()->Network;
    while (!deleteSceneRequest && !changeNextSceneRequest && !changePreviousSceneRequest)
    {
        char buffer[128];
        memset(buffer, -52, 128);
        unsigned long bytesReceived = Network->ReceivePacket(TCP, buffer);

        PacketType packetType = Network->GetPacketType(bytesReceived);
        if (packetType == Type_ChatPacket)
        {
            ChatPacket packet;
            Network->GetPacketFromBytes(buffer, packet);
            PrintMessage(packet.message);
        }
        else if (packetType == Type_ServerMessagePacket)
        {
            ServerMessagePacket packet;
            Network->GetPacketFromBytes(buffer, packet);
            ServerMessage msg = Network->GetServerMessage(packet.msg);
            if (msg == Server_PlayerConnected || msg == Server_PlayerDisconnected)
                UpdateRoomInfo();
            else if (msg == Server_StartBattleScene)
                changeNextSceneRequest = true;
            else if (msg == Server_Denied_NotEnoughPlayers)
            {
                UserServerMessage->SetText("There are not enough players.");
                sfg::Context::Get().GetEngine().SetProperty("Label#ServerMessage", "Color", sf::Color(127, 0, 0, 255));
            }
        } 

        //delete buffer;
        //sf::sleep(sf::milliseconds(16));
    }
}

void WaitRoomScene::PrintMessage(std::string message)
{
    mutex.lock();
    if (Messages.size() == MaxChatMessages)
    {
        ChatWindowMessages->Remove(Messages[0]);
        Messages.erase(Messages.begin());
    }

    sfg::Label::Ptr a = sfg::Label::Create(message);
    a->SetAlignment(sf::Vector2f(0.0f, 0.0f));

    Messages.push_back(a);
    ChatWindowMessages->Pack(Messages[Messages.size() - 1]);
    mutex.unlock();
}

void WaitRoomScene::UpdateRoomInfo()
{
    playerGUIInfo.clear();
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
        unsigned long bytesReceived = Network->ReceivePacket(TCP, buffer);

        //If receiving multiple packets at once
        if (bytesReceived > packetSize)
        {
            int numberOfPackets = (uint)bytesReceived / packetSize;
            for (int i = 0; i < numberOfPackets; i++)
            {
                char bufferPart[packetSize];
                memcpy(bufferPart, buffer + (packetSize * i), packetSize);
                
                PlayerInfoPacket player;
                Network->GetPacketFromBytes(bufferPart, player);
                if (Network->GetPacketType(packetSize) == Type_PlayerInfoPacket)
                {
                    bool me = false;
                    if (player.ID == Network->GetClientID())
                    {
                        Network->SetAuthority(player.authority);
                        me = true;
                    }
                    PlayerGUIInfo newPlayer;
                    newPlayer.username = player.username;
                    newPlayer.wincount = player.wincount;
                    newPlayer.me = me;
                    playerGUIInfo.push_back(newPlayer);
                    numPlayers++;
                }
            }
            allInfoReceived = true;
        }
        else
        {
            if (Network->GetPacketType(bytesReceived) == Type_PlayerInfoPacket)
            {
                PlayerInfoPacket player;
                bool me = false;
                Network->GetPacketFromBytes(buffer, player);
                if (player.ID == Network->GetClientID())
                {
                    Network->SetAuthority(player.authority);
                    allInfoReceived = true;
                    me = true;
                }

                PlayerGUIInfo newPlayer;
                newPlayer.username = player.username;
                newPlayer.wincount = player.wincount;
                newPlayer.me = me;
                playerGUIInfo.push_back(newPlayer);
                numPlayers++;
            }
        }
    }

    updateGUIRequest = true;
}

void WaitRoomScene::UpdateGUI()
{
    UsersWindowBox->RemoveAll();

    for (int i = 0; i < playerGUIInfo.size(); i++)
        AddPlayerInfo(playerGUIInfo[i].username.c_str(), playerGUIInfo[i].wincount, playerGUIInfo[i].me, i);

    std::stringstream text;
    text << "Users in the room: " << playerGUIInfo.size();
    UserText->SetText(text.str());

    UserButtonTable->RemoveAll();

    sfg::Button::Ptr LogOut = sfg::Button::Create("Log Out");
    LogOut->SetRequisition(sf::Vector2f(100.0f, 5.0f));
    LogOut->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&WaitRoomScene::OnLogOutPressed, this));

    if (GameManager::GetInstance()->Network->IsAuthority())
    {
        sfg::Button::Ptr StartGame = sfg::Button::Create("Start Game");
        StartGame->SetRequisition(sf::Vector2f(100.0f, 5.0f));
        StartGame->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&WaitRoomScene::OnStartGamePressed, this));

        UserServerMessage->SetText("Press start to begin the game...");
        UserServerMessage->SetId("ServerMessage");
        sfg::Context::Get().GetEngine().SetProperty("Label#ServerMessage", "Color", sf::Color(255, 255, 255, 255));

        UserButtonTable->Attach(StartGame, sf::Rect<sf::Uint32>(0, 0, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.0f, 0.0f));
        UserButtonTable->Attach(LogOut, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.0f, 0.0f));
    }
    else
    {
        UserServerMessage->SetText("Waiting for the host to start...");
        UserButtonTable->Attach(LogOut, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(0.0f, 0.0f));
    }

    updateGUIRequest = false;
}

void WaitRoomScene::AddPlayerInfo(const char* username, unsigned int wincount, bool me, unsigned int row)
{
    std::stringstream text;
    text << " Wins: " << wincount;

    sfg::Label::Ptr user = sfg::Label::Create(username);
    user->SetAlignment(sf::Vector2f(0.0f, 0.0f));
    if (me)
    {
        user->SetId("User");
        sfg::Context::Get().GetEngine().SetProperty("Label#User", "Color", sf::Color(255, 255, 0, 255));
    }
    sfg::Label::Ptr wins = sfg::Label::Create(text.str());
    wins->SetAlignment(sf::Vector2f(1.0f, 0.0f));

    UsersWindowBox->Attach(user, sf::Rect<sf::Uint32>(0, row, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);
    UsersWindowBox->Attach(wins, sf::Rect<sf::Uint32>(1, row, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(30.0f, 0.0f));
}
