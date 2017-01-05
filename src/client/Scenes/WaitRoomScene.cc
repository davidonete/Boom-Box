#include "Scenes/WaitRoomScene.h"
#include "System/GameManager.h"

WaitRoomScene::WaitRoomScene() {}

WaitRoomScene::~WaitRoomScene()
{
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

    auto Window = sfg::Window::Create(sfg::Window::BACKGROUND | sfg::Window::SHADOW);

    auto button = sfg::Button::Create("Send");
    button->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&WaitRoomScene::OnSendPressed, this));

    ScrolledWindowBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

    auto scrolledwindow = sfg::ScrolledWindow::Create();
    scrolledwindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC);
    scrolledwindow->AddWithViewport(ScrolledWindowBox);
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

    Window->Add(box);

    sf::Vector2f loginSize = Window->GetRequisition();
    sf::Vector2f pos = sf::Vector2f((winSize.x - loginSize.x) * ChatPosX, (winSize.y - loginSize.y) * ChatPosY);
    Window->SetPosition(pos);

    Desktop.Add(Window);

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
        ChatPacket packet;
        char buffer[128];
        Network->ReceivePacket(TCP, buffer);
        if (Network->GetPacketFromBytes(buffer, packet))
            PrintMessage(packet.message);

        sf::sleep(sf::milliseconds(16));
    }
}

void WaitRoomScene::PrintMessage(std::string message)
{
    mutex.lock();
    if (Messages.size() == MaxChatMessages)
    {
        ScrolledWindowBox->Remove(Messages[0]);
        Messages.erase(Messages.begin());
    }

    sfg::Label::Ptr a = sfg::Label::Create(message);
    a->SetAlignment(sf::Vector2f(0.0f, 0.0f));

    Messages.push_back(a);
    ScrolledWindowBox->Pack(Messages[Messages.size() - 1]);
    mutex.unlock();
}