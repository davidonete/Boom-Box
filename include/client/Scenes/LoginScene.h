#include "Scenes/Scene.h"

class LoginScene : public Scene
{
public:
    LoginScene();
    ~LoginScene();

    void Init() override;
    void Input() override;
    void Update() override;
    void Render() override;

private:
    sfg::SFGUI GUI;
    sfg::Window::Ptr Window;
    sf::RenderWindow* RenderWindow;
    sf::Clock GUIClock;

    sf::Texture background;
    sf::Sprite sprite;
};