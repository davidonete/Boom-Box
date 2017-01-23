#ifndef LOGINSCENE_H
#define LOGINSCENE_H

#include "Scenes/Scene.h"

/** Login scene.
 *  The scene to log in to the server.
 */
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
  /** Initialize the GUI. */
  void InitGUI();
  /** Function called when the player presses the log in button. */
  void OnLoginPressed();
  /** Function called when the player presses the exit button or closes the window. */
  void OnExitPressed();
    
  /** Prints a log in error message received from the server.
   *  @param message The message received.
   */
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
