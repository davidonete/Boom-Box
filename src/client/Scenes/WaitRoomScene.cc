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
    bgScale.y = (winSize.y * 1.0f) / 600.0f;
    sprite.setScale(bgScale);

    //Receive server packets
    serverReceiveThread = new sf::Thread(std::bind(&WaitRoomScene::GetServerPackets, this));
    serverReceiveThread->launch();
}

void WaitRoomScene::InitGUI()
{
    sf::Vector2f winSize = (sf::Vector2f)RenderWindow->getSize();
    sf::Vector2f ChatSize(((winSize.x * 500.f) / 1366.0f) * ChatSizeX, ((winSize.y * 160.f) / 768.0f) * ChatSizeY);
    MaxChatMessages = (int)((ChatSize.y * 10) / 160.0f);

    auto ChatWindow = sfg::Window::Create(sfg::Window::BACKGROUND | sfg::Window::SHADOW);

    auto button = sfg::Button::Create("Send");
    button->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&WaitRoomScene::OnSendPressed, this));

    ChatWindowBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

    auto scrolledwindow = sfg::ScrolledWindow::Create();
    scrolledwindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC);
    scrolledwindow->AddWithViewport(ChatWindowBox);
    scrolledwindow->SetRequisition(ChatSize);

    TextBox = sfg::Entry::Create();
    TextBox->SetMaximumLength(45);
    TextBox->SetRequisition(sf::Vector2f(100.0f, 20.0f));

    sfg::Table::Ptr Table = sfg::Table::Create();
    Table->Attach(TextBox, sf::Rect<sf::Uint32>(0, 0, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);
    Table->Attach(button, sf::Rect<sf::Uint32>(1, 0, 1, 1), sfg::Table::FILL, sfg::Table::FILL);

    auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    box->Pack(scrolledwindow, false, true);
    box->Pack(Table, false, true);

    ChatWindow->Add(box);

    sf::Vector2f loginSize = ChatWindow->GetRequisition();
    sf::Vector2f pos = sf::Vector2f((winSize.x - loginSize.x) * ChatPosX, (winSize.y - loginSize.y) * ChatPosY);
    ChatWindow->SetPosition(pos);

    Desktop.Add(ChatWindow);

    sf::Vector2f UserSize(((winSize.x * 500.f) / 1366.0f) * UserSizeX, ((winSize.y * 160.f) / 768.0f) * UserSizeY);

    auto UserWindow = sfg::Window::Create(sfg::Window::BACKGROUND | sfg::Window::SHADOW);
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

    auto box2 = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    box2->Pack(UserText);
    box2->Pack(userscrolledwindow, false, true);

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
    RenderWindow->draw(sprite);
    GUI.Display(*RenderWindow);

    if (deleteSceneRequest || changeSceneRequest)
    {
        //serverReceiveThread->wait();
        if(deleteSceneRequest)
            GameManager::GetInstance()->CloseGame();
        else if(changeSceneRequest)
            GameManager::GetInstance()->ChangeScene(GameScene_Battle);
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

        //PrintMessage(buffer.str());

        ChatPacket packet;
        packet.id = GM->Network->GetClientID();
        std::strcpy(packet.message, buffer.str().c_str());
        GM->Network->SendPacket(packet);
    }
}

void WaitRoomScene::GetServerPackets()
{
    NetworkManager* Network = GameManager::GetInstance()->Network;
    while (!deleteSceneRequest && !changeSceneRequest)
    {
        char buffer[128];
        Network->ReceivePacket(TCP, buffer);

        PacketType packetType = Network->GetPacketType(buffer);
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
            if(msg == Server_PlayerConnected || msg == Server_PlayerDisconnected)
                UpdateRoomInfo();
        } 

        sf::sleep(sf::milliseconds(16));
    }
}

void WaitRoomScene::PrintMessage(std::string message)
{
    mutex.lock();
    if (Messages.size() == MaxChatMessages)
    {
        ChatWindowBox->Remove(Messages[0]);
        Messages.erase(Messages.begin());
    }

    sfg::Label::Ptr a = sfg::Label::Create(message);
    a->SetAlignment(sf::Vector2f(0.0f, 0.0f));

    Messages.push_back(a);
    ChatWindowBox->Pack(Messages[Messages.size() - 1]);
    mutex.unlock();
}

void WaitRoomScene::UpdateRoomInfo()
{
    UsersWindowBox->RemoveAll();
    NetworkManager* Network = GameManager::GetInstance()->Network;

    ClientRequestPacket request;
    request.ID = Network->GetClientID();
    request.msg = Network->GetCodeFromMessage(Request_GetPlayersInfo);
    Network->SendPacket(TCP, request);

    const int packetSize = sizeof(PlayerInfoPacket);
    bool allInfoReceived = false;
    int numPlayers = 0;
    while (!allInfoReceived)
    {
        char buffer[128];
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
                    AddPlayerInfo(player.username, player.wincount, numPlayers);
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
                if (player.id == Network->GetClientID())
                    allInfoReceived = true;

                AddPlayerInfo(player.username, player.wincount, numPlayers);
                numPlayers++;
            }
        }
    }

    std::stringstream text;
    text << "Users in the room: " << numPlayers;
    UserText->SetText(text.str());
}

void WaitRoomScene::AddPlayerInfo(char* username, unsigned int wincount, unsigned int row)
{
    mutex.lock();
    std::stringstream text;
    text << " Wins: " << wincount;

    sfg::Label::Ptr user = sfg::Label::Create(username);
    user->SetAlignment(sf::Vector2f(0.0f, 0.0f));
    sfg::Label::Ptr wins = sfg::Label::Create(text.str());
    wins->SetAlignment(sf::Vector2f(1.0f, 0.0f));

    UsersWindowBox->Attach(user, sf::Rect<sf::Uint32>(0, row, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);
    UsersWindowBox->Attach(wins, sf::Rect<sf::Uint32>(1, row, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL, sf::Vector2f(30.0f, 0.0f));
    mutex.unlock();
}