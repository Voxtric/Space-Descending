//  *********************************************************************************
#pragma once
#include "errortype.h"
#include "windows.h"
#include "ObjectManager.h"
#include "event.h"

#include "GameManager.h"
#include "GUI.h"

// For reading keyboard
#define KEYPRESSED(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

class Game
{
private:
  static SoundIndex s_chooseSound;
  static SoundIndex s_navigateSound[3];
  static unsigned int s_navigateSoundIndex;
  static SoundIndex s_backgroundSound;
  static SoundIndex s_gameBackgroundSound;

	enum GameState{MENU, PAUSED, RUNNING, GAMEOVER, SCORE, HOWTOPLAY};
	GameState m_currentState;
	void ChangeState(GameState newState);
	int m_menuOption;
  Voxtric::GameManager* m_gameManager;
  Voxtric::GUI m_GUI;
  bool m_backgroundSoundPlaying;
  bool m_gameBackgroundSoundPlaying;

	Game();
	~Game();
	Game(Game& other);

public:
	static Game instance;
	ObjectManager m_objects;
	ErrorType Setup(bool bFullScreen, HWND hwnd, HINSTANCE hinstance);
	void Shutdown();
	ErrorType Main();
  void ShowScores();
  void ToMainMenu();

	ErrorType PauseMenu();
	ErrorType MainMenu();
	ErrorType StartOfGame();
	ErrorType Update();
  ErrorType EndOfGame();
  void NotifyEvent(Event evt);
  Voxtric::GameManager& GetGameManager();
  Voxtric::GUI& GetGUI();
};




