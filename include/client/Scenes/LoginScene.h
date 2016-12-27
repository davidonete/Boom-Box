//#include <SFGUI/SFGUI.hpp>
//#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>

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
    //sfg::SFGUI GUI;
    //std::shared_ptr<sfg::Window> Window;
    sf::RenderWindow* RenderWindow;
};