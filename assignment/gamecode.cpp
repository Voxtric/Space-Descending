// GameCode.cpp		


// Version  13.03	Draw Engine no longer requires a height / width
// Version 13.0   Update to wchar_t and wstring

//Version 11			5/3/08	
// These three functions form the basis of a game loop in the window created in the
// wincode.cpp file

#include "gamecode.h"
#include "mydrawengine.h"
#include "mysoundengine.h"
#include "myinputs.h"
#include <time.h>
#include "gametimer.h"
#include "errorlogger.h"
#include <math.h>
#include "shapes.h"
#include "ObjectManager.h"
#include "entities.h"
#include "Building.h"
#include "Helicopter.h"
#include "Turret.h"
#include "Plane.h"

SoundIndex Game::s_chooseSound = 0;
SoundIndex Game::s_navigateSound[3];
unsigned int Game::s_navigateSoundIndex = 0;
SoundIndex Game::s_backgroundSound = 0;
SoundIndex Game::s_gameBackgroundSound = 0;

Voxtric::GameManager& Game::GetGameManager()
{
  return *m_gameManager;
}

void Game::ShowScores()
{
  m_currentState = SCORE;
  m_gameManager->UpdateHighScores();
}

ErrorType Game::Main()
// Called repeatedly - the game loop
{
	//Flip and clear the back buffer
	MyDrawEngine* pTheDrawEngine= MyDrawEngine::GetInstance();
	pTheDrawEngine->Flip();
	pTheDrawEngine->ClearBackBuffer();

  static bool l_loadSound = true;
  if (l_loadSound)
  {
    s_chooseSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/menu_choose.wav");
    for (unsigned int i = 0; i < 3; ++i)
    {
      s_navigateSound[i] = MySoundEngine::GetInstance()->LoadWav(L"Sounds/menu_navigate.wav");
    }
    l_loadSound = false;
  }

	ErrorType err;

	switch(m_currentState)
	{
	case MENU:
    if (m_gameBackgroundSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_gameBackgroundSound);
      MySoundEngine::GetInstance()->Unload(s_gameBackgroundSound);
      m_gameBackgroundSoundPlaying = false;
    }
		err= MainMenu();
		break;	
  
  case PAUSED:
		err = PauseMenu();
		break;

	case RUNNING:
    if (m_backgroundSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_backgroundSound);
      MySoundEngine::GetInstance()->Unload(s_backgroundSound);
      m_backgroundSoundPlaying = false;
    }

		err= Update();
		break;

  case HOWTOPLAY:
    if (!m_backgroundSoundPlaying)
    {
      s_backgroundSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/menu_background.wav");
      MySoundEngine::GetInstance()->Play(s_backgroundSound, true);
      m_backgroundSoundPlaying = true;
    }
    if (m_gameBackgroundSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_gameBackgroundSound);
      MySoundEngine::GetInstance()->Unload(s_gameBackgroundSound);
      m_gameBackgroundSoundPlaying = false;
    }
    m_objects.DrawAll();
    m_objects.OnPauseAll();
    m_GUI.DrawHowToPlay();
    err = SUCCESS;
    break;

  case SCORE:
    if (!m_backgroundSoundPlaying)
    {
      s_backgroundSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/menu_background.wav");
      MySoundEngine::GetInstance()->Play(s_backgroundSound, true);
      m_backgroundSoundPlaying = true;
    }
    if (m_gameBackgroundSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_gameBackgroundSound);
      MySoundEngine::GetInstance()->Unload(s_gameBackgroundSound);
      m_gameBackgroundSoundPlaying = false;
    }
    m_objects.DrawAll();
    m_objects.OnPauseAll();
    m_GUI.DrawScoreMenu();
    err = SUCCESS;
    break;

	case GAMEOVER:
		err = FAILURE;
    break;
	default:
		// Not a valid state
		err = FAILURE;
	}

	return err;
}

void Game::ChangeState(GameState newState)
{
	// Very crude state system
	// Close old state
	switch(m_currentState)
	{
	case MENU:

		break;
	case PAUSED:

		break;
	case RUNNING:

		break;
	}

	// Change the state
	m_currentState = newState;
	m_menuOption = 0;

	// Transition to new state
	switch(m_currentState)
	{
	case MENU:

		break;
	case PAUSED:

		break;
	case RUNNING:

		break;
	}
}

ErrorType Game::Setup(bool bFullScreen, HWND hwnd, HINSTANCE hinstance)
// Called once before entering game loop. 
{
  m_backgroundSoundPlaying = false;
  m_gameBackgroundSoundPlaying = false;
	// Create the engines - this should be done before creating other DDraw objects
	if(FAILED(MyDrawEngine::Start(hwnd, bFullScreen)))
	{
		ErrorLogger::Writeln(L"Failed to start MyDrawEngine");
		return FAILURE;
	}
	if(FAILED(MySoundEngine::Start(hwnd)))
	{
		ErrorLogger::Writeln(L"Failed to start MySoundEngine");
		return FAILURE;
	}
	if(FAILED(MyInputs::Start(hinstance, hwnd)))
	{
		ErrorLogger::Writeln(L"Failed to start MyInputs");
		return FAILURE;
	}
	return (SUCCESS);
}

void Game::Shutdown()
// Called once before entering game loop. 
{
	m_objects.DeleteAllObjects();
  RemoveFontResource(L"Fonts/game_font.ttf");

	// (engines must be terminated last)
	MyDrawEngine::Terminate();
	MySoundEngine::Terminate();
	MyInputs::Terminate();
}
// **********************************************************************************************
// The game !!! *********************************************************************************
// **********************************************************************************************




void Game::ToMainMenu()
{
  EndOfGame();
  ChangeState(MENU);
}

ErrorType Game::PauseMenu()
{
	// Code for a basic pause menu
  m_objects.OnPauseAll();
  m_objects.DrawAll();
  m_GUI.DrawPauseMenu(m_menuOption);

	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard();
	if(pInputs->NewKeyPressed(DIK_UP) && m_menuOption > 0)
	{
    MySoundEngine::GetInstance()->Play(s_navigateSound[s_navigateSoundIndex]);
    s_navigateSoundIndex = s_navigateSoundIndex == 2 ? 0 : s_navigateSoundIndex + 1;
		m_menuOption--;
	}
	if(pInputs->NewKeyPressed(DIK_DOWN) && m_menuOption < 2 - 1)
  {
    MySoundEngine::GetInstance()->Play(s_navigateSound[s_navigateSoundIndex]);
    s_navigateSoundIndex = s_navigateSoundIndex == 2 ? 0 : s_navigateSoundIndex + 1;
		m_menuOption++;
	}

	if(pInputs->NewKeyPressed(DIK_RETURN))
  {
    MySoundEngine::GetInstance()->Play(s_chooseSound);
		if(m_menuOption ==0)
		{
			ChangeState(RUNNING);
		}
		if(m_menuOption ==1)
		{
			EndOfGame();
			ChangeState(MENU);
		}

	}

	return SUCCESS;
}

Voxtric::GUI& Game::GetGUI()
{
  return m_GUI;
}

ErrorType Game::MainMenu()
{
	// Code for a basic main menu
  if (!m_backgroundSoundPlaying)
  {
    s_backgroundSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/menu_background.wav");
    MySoundEngine::GetInstance()->Play(s_backgroundSound, true);
    m_backgroundSoundPlaying = true;
  }  

  m_GUI.DrawMainMenu(m_menuOption);

	MyInputs* pInputs = MyInputs::GetInstance();

	pInputs->SampleKeyboard();
	if(pInputs->NewKeyPressed(DIK_UP) && m_menuOption > 0)
  {
    MySoundEngine::GetInstance()->Play(s_navigateSound[s_navigateSoundIndex]);
    s_navigateSoundIndex = s_navigateSoundIndex == 2 ? 0 : s_navigateSoundIndex + 1;
		m_menuOption--;
	}
	if(pInputs->NewKeyPressed(DIK_DOWN) && m_menuOption < 5 - 1)
  {
    MySoundEngine::GetInstance()->Play(s_navigateSound[s_navigateSoundIndex]);
    s_navigateSoundIndex = s_navigateSoundIndex == 2 ? 0 : s_navigateSoundIndex + 1;
		m_menuOption++;
	}

	if(pInputs->NewKeyPressed(DIK_RETURN))
	{
    MySoundEngine::GetInstance()->Play(s_chooseSound);
		if(m_menuOption ==0)
		{
			StartOfGame();
			ChangeState(RUNNING);
		}
    else if (m_menuOption == 1)
    {
      StartOfGame();
      ChangeState(SCORE);
    }
    else if (m_menuOption == 2)
    {
      StartOfGame();
      ChangeState(HOWTOPLAY);
    }
		else if(m_menuOption == 3)
		{
			if(MyDrawEngine::GetInstance()->IsWindowFullScreen())
			{
				MyDrawEngine::GetInstance()->GoWindowed();
			}
			else
			{
				MyDrawEngine::GetInstance()->GoFullScreen();
			}
		}
		else if(m_menuOption ==4)
		{
			ChangeState(GAMEOVER);
		}
	}

	return SUCCESS;
}

ErrorType Game::StartOfGame()
{
	// Code to set up your game
  m_gameManager = new Voxtric::GameManager();
  m_objects.AddItem(m_gameManager, false);

  if (!m_gameBackgroundSoundPlaying)
  {
    s_gameBackgroundSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/game_background.wav");
    MySoundEngine::GetInstance()->Play(s_gameBackgroundSound, true);
    m_gameBackgroundSoundPlaying = true;
  }

  m_gameManager->StartGame();
	return SUCCESS;
}

ErrorType Game::Update()
{
  m_objects.UpdateAll();
  m_objects.ProcessCollisions();
	m_objects.DrawAll();

  MyDrawEngine* pDrawEngine = MyDrawEngine::GetInstance();

	// Garbage collect any objects. Possibly slow
	m_objects.DeleteInactiveItems();

	// Check for entry to pause menu
	static bool escapepressed = true;
	if(KEYPRESSED(VK_ESCAPE))
	{
		if(!escapepressed)
			ChangeState(PAUSED);
		escapepressed=true;
	}
	else
		escapepressed=false;

	// Any code here to run the game,
	// but try to do this within a game object if possible

	return SUCCESS;
}

ErrorType Game::EndOfGame()
// called when the game ends by returning to main menu
{
	m_objects.DeleteAllObjects();

	return SUCCESS;
}

Game::Game()
{
	// No-op
}

Game::~Game()
{
	// No-op
}


void Game::NotifyEvent(Event evt)
{
	m_objects.HandleEvent(evt);
}

Game Game::instance;