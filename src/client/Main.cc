#include "System/GameManager.h"

void Init(GameManager* GM)
{
  GM->ChangeScene(GameScene_Battle);
}

void Exit(GameManager* GM)
{
  delete GM;
}

int main()
{
  GameManager* GM = GameManager::GetInstance();

  Init(GM);
  while (GM->GetWindow()->isOpen())
  {
    GM->Input();
    GM->Update();
    GM->Render();
  }

  Exit(GM);
  return EXIT_SUCCESS;
}