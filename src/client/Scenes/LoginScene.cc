#include "Scenes/LoginScene.h"
#include "System/GameManager.h"

LoginScene::LoginScene() {}

LoginScene::~LoginScene() {}

void LoginScene::Init() 
{
    RenderWindow = GameManager::GetInstance()->GetWindow();

    sf::Texture::getMaximumSize();
    sf::Shader::isAvailable();

    sfg::Renderer::Set(sfg::VertexArrayRenderer::Create());

    RenderWindow->resetGLStates();

    Window = sfg::Window::Create(sfg::Window::BACKGROUND | sfg::Window::SHADOW);

    sfg::Entry::Ptr username_entry = sfg::Entry::Create();
    username_entry->SetMaximumLength(32);
    username_entry->SetRequisition(sf::Vector2f(100.0f, 0.0f));

    sfg::Entry::Ptr password = sfg::Entry::Create();
    password->HideText('*');
    password->SetMaximumLength(32);
    password->SetRequisition(sf::Vector2f(100.0f, 0.0f));

    sfg::Button::Ptr LogIn = sfg::Button::Create("Log In");
    LogIn->SetRequisition(sf::Vector2f(100.0f, 5.0f));
    LogIn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&LoginScene::OnLoginPressed, this));

    sfg::Button::Ptr Exit = sfg::Button::Create("Exit");
    Exit->SetRequisition(sf::Vector2f(100.0f, 5.0f));
    Exit->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&LoginScene::OnExitPressed, this));

    sfg::Table::Ptr m_table = sfg::Table::Create();
    m_table->Attach(sfg::Label::Create(L"Username:"), sf::Rect<sf::Uint32>(0, 1, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
    m_table->Attach(username_entry, sf::Rect<sf::Uint32>(1, 1, 1, 1), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);
    m_table->Attach(sfg::Label::Create(L"Password:"), sf::Rect<sf::Uint32>(0, 2, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
    m_table->Attach(password, sf::Rect<sf::Uint32>(1, 2, 1, 1), sfg::Table::FILL, sfg::Table::FILL);
    m_table->Attach(LogIn, sf::Rect<sf::Uint32>(0, 3, 1, 1), sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.0f, 0.0f));
    m_table->Attach(Exit, sf::Rect<sf::Uint32>(1, 3, 1, 1), sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(0.0f, 0.0f));
    m_table->SetRowSpacings(5.f);
    m_table->SetColumnSpacings(5.f);
    
    // Pack into box
    sfg::Box::Ptr Box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    Box->Pack(m_table, true);

    // Set box spacing
    Box->SetSpacing(5.f);

    // Add our box to the window
    Window->Add(Box);
    sf::Vector2f loginsize = Window->GetRequisition();
    sf::Vector2u winSize = RenderWindow->getSize();
    sf::Vector2f pos = sf::Vector2f((winSize.x * 0.5f) - (loginsize.x * 0.5f), (winSize.y * 0.5f) - (loginsize.y * 0.5f));
    Window->SetPosition(pos);

    //Background
    background.loadFromFile(GameManager::GetImagePath("background.jpg"));
    sprite.setTexture(background);
    sf::Vector2f bgScale;
    bgScale.x = (winSize.x * 0.5f) / 800.0f;
    bgScale.y = (winSize.y * 1.0f) / 600.0f;
    sprite.setScale(bgScale);
}

void LoginScene::Input() {}

void LoginScene::Update() 
{
    sf::Event event;
    while (RenderWindow->pollEvent(event))
    {
        Window->HandleEvent(event);
        if (event.type == sf::Event::Closed)
            RenderWindow->close();
    }
    
    Window->Update(GUIClock.getElapsedTime().asSeconds());
}

void LoginScene::Render() 
{
    RenderWindow->draw(sprite);
    GUI.Display(*RenderWindow);
}

void LoginScene::OnLoginPressed()
{
    std::cout << "Login Pressed\n";
}

void LoginScene::OnExitPressed()
{
    GameManager::GetInstance()->CloseGame();
}
