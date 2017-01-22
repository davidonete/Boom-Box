#ifndef LOGINSCENE_H
#define LOGINSCENE_H

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
  void InitGUI();
  void OnLoginPressed();
  void OnExitPressed();
  void LoginError(sf::String message);

  sfg::SFGUI GUI;
  sfg::Window::Ptr Window;
  sf::RenderWindow* RenderWindow;
  sf::Clock GUIClock;

  sf::Texture background;
  sf::Sprite sprite;

  sfg::Label::Ptr ErrorMsg;
  sfg::Entry::Ptr IP;
  sfg::Entry::Ptr Port;
  sfg::Entry::Ptr Username;
  sfg::Entry::Ptr Password;

  bool changeSceneRequest = false;
  bool deleteSceneRequest = false;
  bool alreadyConnected = false;
};

#endif
