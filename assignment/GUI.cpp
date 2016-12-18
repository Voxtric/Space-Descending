#include "GUI.h"

#include "mydrawengine.h"
#include "Building.h"
#include "gamecode.h"

namespace Voxtric
{
  const float GUI::BACKGROUND_TILE_SIZE = 1024.0f;

  void GUI::DrawMenuBackground()
  {
    MyDrawEngine::GetInstance()->theCamera.PlaceAt(Vector2D(0.0f, 0.0f));
    static bool l_makeBuildings = true;
    static Voxtric::Building* l_buildings[BACKGROUND_BUILDINGS];
    if (l_makeBuildings)
    {
      //Placed slightly before the start of the screen.
      Vector2D position = MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft() - 
        Vector2D(GameManager::DESPAWN_BUFFER_DISTANCE, 0.0f);
      for (unsigned int i = 0; i < BACKGROUND_BUILDINGS; ++i)
      {
        l_buildings[i] = new Voxtric::Building(position);
        position.XValue += l_buildings[i]->GetDimensions().XValue;
      }
      l_makeBuildings = false;
    }
    DrawTiledBackground(false);
    //Draws all the background buildings.
    for (unsigned int i = 0; i < BACKGROUND_BUILDINGS; ++i)
    {
      l_buildings[i]->Draw();
    }

    //Drawing random buildings all over the menu.
    static const PictureIndex HELICOPTER = MyDrawEngine::GetInstance()->LoadPicture(
      L"Textures/helicopter/helicopter_0.bmp");
    static const PictureIndex PLANE = MyDrawEngine::GetInstance()->LoadPicture(
      L"Textures/plane.bmp");												   					   
	  Vector2D screenRect = Vector2D((float)MyDrawEngine::GetInstance()->GetScreenWidth(), 
								                   (float)MyDrawEngine::GetInstance()->GetScreenHeight());
    MyDrawEngine::GetInstance()->DrawAt(
      Vector2D(screenRect.XValue * 0.15f, screenRect.YValue * -0.3f), HELICOPTER);
    MyDrawEngine::GetInstance()->DrawAt(
      Vector2D(screenRect.XValue * -0.5f, screenRect.YValue * 0.3f), HELICOPTER);
    MyDrawEngine::GetInstance()->DrawAt(
      Vector2D(screenRect.XValue * 0.6f, 0.0f), PLANE);
  }

  void GUI::DrawTiledBackground(bool useParalax)
  {
    Rectangle2D viewPort = MyDrawEngine::GetInstance()->GetViewport();
    static const PictureIndex BACKGROUND = MyDrawEngine::GetInstance()->LoadPicture(
      L"Textures/background.bmp");
    if (!useParalax)
    {
      //Tiles from just off the left of the screen until just off the right of the screen.
      for (float x = viewPort.GetTopLeft().XValue - 
        (float)((int)(viewPort.GetTopLeft().XValue) % (int)BACKGROUND_TILE_SIZE) 
        - BACKGROUND_TILE_SIZE; x < viewPort.GetTopRight().XValue; x += BACKGROUND_TILE_SIZE)
      {
        //Tiles from just off the bottom of the screen until just off the top of the screen.
        for (float y = viewPort.GetBottomLeft().YValue - 
          (float)((int)(viewPort.GetBottomLeft().YValue) % (int)BACKGROUND_TILE_SIZE) -
          BACKGROUND_TILE_SIZE; y < viewPort.GetTopLeft().YValue; y += BACKGROUND_TILE_SIZE)
        {
          MyDrawEngine::GetInstance()->DrawAt(
            Vector2D(x + BACKGROUND_TILE_SIZE * 0.5f, y + BACKGROUND_TILE_SIZE * 0.5f),BACKGROUND);
        }
      }
    }
    else
    {
      //Tiles from just off the left of the screen until just off the right of the screen, also 
      //taking into account the paralax offset created by player movement.
      for (float x = viewPort.GetTopLeft().XValue - 
        (float)((int)(viewPort.GetTopLeft().XValue +
        Game::instance.GetGameManager().GetParalaxOffset().XValue) % (int)BACKGROUND_TILE_SIZE) - 
        BACKGROUND_TILE_SIZE; x < viewPort.GetTopRight().XValue; x += BACKGROUND_TILE_SIZE)
      {
        //Tiles from just off the bottom of the screen until just off the top of the screen, also 
        //taking into account the paralax offset created by player movement.
        for (float y = viewPort.GetBottomLeft().YValue -
          (float)((int)(viewPort.GetBottomLeft().YValue +
          Game::instance.GetGameManager().GetParalaxOffset().YValue) % (int)BACKGROUND_TILE_SIZE) - 
          BACKGROUND_TILE_SIZE; y < viewPort.GetTopLeft().YValue; y += BACKGROUND_TILE_SIZE)
        {
          MyDrawEngine::GetInstance()->DrawAt(
            Vector2D(x + BACKGROUND_TILE_SIZE * 0.5f, y + BACKGROUND_TILE_SIZE * 0.5f),BACKGROUND);
        }
      }
    }
  }

  void GUI::DrawMainMenu(int menuOption)
  {
    DrawMenuBackground();
    static const FontIndex TITLE_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", MAIN_TITLE_FONT_SIZE, true, false);
    static const FontIndex TEXT_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", TEXT_FONT_SIZE, false, false);
	
	  Vector2D screenRect = Vector2D((float)MyDrawEngine::GetInstance()->GetScreenWidth(), 
                                   (float)MyDrawEngine::GetInstance()->GetScreenHeight());
	  MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 450, 
      (int)(screenRect.YValue * 0.2f), L"Space Descending", MyDrawEngine::WHITE, TITLE_FONT);
    const int NUMOPTIONS = 5;
    wchar_t options[NUMOPTIONS][17] = 
    { L"Start game", L"View High Scores", L"How To Play", L"Go Fullscreen", L"Exit" };

    if (MyDrawEngine::GetInstance()->IsWindowFullScreen())
    {
      wcscpy_s(options[3], wcslen(L"Go Windowed") + 1, L"Go Windowed");
    }

    //Draws all menu options.
    for (int i = 0; i<NUMOPTIONS; i++)
    {
      int colour = MyDrawEngine::GREY;
      if (i == menuOption)
      {
        colour = MyDrawEngine::WHITE;
        static const PictureIndex TURRET_IMAGE = MyDrawEngine::GetInstance()->LoadPicture(
          L"Textures/turret.bmp");
        //Draws turret at the chosen menu option.
        Vector2D turretPosition = MyDrawEngine::GetInstance()->theCamera.ReverseTransform(
			    Vector2D((screenRect.XValue * 0.5f) - 520.0f,
                   (screenRect.YValue * 0.2f) + (50 * (i + 3)) + 28));
        MyDrawEngine::GetInstance()->DrawAt(turretPosition, TURRET_IMAGE, 1.8f, 4.71239f);
      }
	    MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 450, 
        (int)(screenRect.YValue * 0.2f) + (50 * (i + 3)), options[i], colour, TEXT_FONT);
    }
  }

  void GUI::DrawPauseMenu(int menuOption)
  {
    static const FontIndex TITLE_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", TITLE_FONT_SIZE, true, false);
    static const FontIndex TEXT_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", TEXT_FONT_SIZE, false, false);
    
    Vector2D screenRect = Vector2D((float)MyDrawEngine::GetInstance()->GetScreenWidth(),
                                   (float)MyDrawEngine::GetInstance()->GetScreenHeight());
    MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 150,
      (int)(screenRect.YValue * 0.3f), L"Paused", MyDrawEngine::WHITE, TITLE_FONT);
    const int NUMOPTIONS = 2;
    wchar_t options[NUMOPTIONS][11] = { L"Resume", L"Main menu" };
    //Draws all menu options.
    for (int i = 0; i<NUMOPTIONS; i++)
    {
      int colour = MyDrawEngine::GREY;
      if (i == menuOption)
      {
        colour = MyDrawEngine::WHITE;
        static const PictureIndex TURRET_IMAGE = MyDrawEngine::GetInstance()->LoadPicture(
          L"Textures/turret.bmp");
        //Draw turret at chosen menu option location.
        Vector2D turretPosition = MyDrawEngine::GetInstance()->theCamera.ReverseTransform(
          Vector2D((screenRect.XValue * 0.5f) - 220.0f,
          (screenRect.YValue * 0.3f) + (50 * (i + 2)) + 28));
        MyDrawEngine::GetInstance()->DrawAt(turretPosition, TURRET_IMAGE, 2.0f, 4.71239f);
      }
      MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 150,
        (int)(screenRect.YValue * 0.3f) + (50 * (i + 2)), options[i], colour, TEXT_FONT);
    }
  }

  void GUI::DrawScoreMenu()
  {
    static const FontIndex TITLE_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", TITLE_FONT_SIZE, true, false);
    static const FontIndex TEXT_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", TEXT_FONT_SIZE, false, false);

    Vector2D screenRect = Vector2D((float)MyDrawEngine::GetInstance()->GetScreenWidth(),
                                   (float)MyDrawEngine::GetInstance()->GetScreenHeight());
    MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 250,
      (int)(screenRect.YValue * 0.2f), L"High Scores", MyDrawEngine::WHITE, TITLE_FONT);
    unsigned int i = 0;
    //Draws all scores.
    for (i; i < GameManager::SCORES_TO_SAVE; ++i)
    {
      unsigned int colour = MyDrawEngine::GREY;
      if (Game::instance.GetGameManager().GetHighScores()[i] ==
          Game::instance.GetGameManager().GetPlayer().GetScore())
      {
        colour = MyDrawEngine::DARKRED;
        static const PictureIndex TURRET_IMAGE = MyDrawEngine::GetInstance()->LoadPicture(
          L"Textures/turret.bmp");
        //Draws turret at the player's score.
        Vector2D turretPosition = MyDrawEngine::GetInstance()->theCamera.ReverseTransform(
          Vector2D((screenRect.XValue * 0.5f) - 320.0f, 
          (screenRect.YValue * 0.2f) + (50 * (i + 2)) + 28));
        MyDrawEngine::GetInstance()->DrawAt(turretPosition, TURRET_IMAGE, 1.8f, 4.71239f);

      }
      MyDrawEngine::GetInstance()->WriteInt((int)(screenRect.XValue * 0.5f) - 250,
        (int)(screenRect.YValue * 0.2f) + (50 * (i + 2)),
        Game::instance.GetGameManager().GetHighScores()[i], colour, TEXT_FONT);
    }
    MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 320,
      (int)(screenRect.YValue * 0.2f) + (50 * (i + 3)),
      L"Press enter to continue", MyDrawEngine::WHITE, TEXT_FONT);

    MyInputs::GetInstance()->SampleKeyboard();
    if (MyInputs::GetInstance()->NewKeyPressed(DIK_RETURN))
    {
      static const SoundIndex CHOOSE_SOUND = MySoundEngine::GetInstance()->LoadWav(
        L"Sounds/menu_choose.wav");
      MySoundEngine::GetInstance()->Play(CHOOSE_SOUND);
      Game::instance.ToMainMenu();
    }
  }

  void GUI::DrawHowToPlay()
  {
    static std::vector<std::wstring> l_lines;
    static bool l_loadHowToPlay = true;
    if (l_loadHowToPlay)
    {
      std::wifstream file("How To Play.txt");
      std::wstring line;
      while (std::getline(file, line))
      {
        l_lines.emplace_back(line);
      }
      l_loadHowToPlay = false;
    }

    static const FontIndex TITLE_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", TITLE_FONT_SIZE, true, false);
    static const FontIndex TEXT_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", SMALL_TEXT_FONT_SIZE, false, false);

    Vector2D screenRect = Vector2D((float)MyDrawEngine::GetInstance()->GetScreenWidth(),
                                   (float)MyDrawEngine::GetInstance()->GetScreenHeight());
    MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 250,
      (int)(screenRect.YValue * 0.2f), L"How To Play", MyDrawEngine::WHITE, TITLE_FONT);
    unsigned int i = 0;
    for (i; i < l_lines.size(); ++i)
    {
      MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 790,
        (int)(screenRect.YValue * 0.2f) + (25 * (i + 4)), l_lines[i].c_str(), MyDrawEngine::GREY, 
        TEXT_FONT);
    }
    MyDrawEngine::GetInstance()->WriteText((int)(screenRect.XValue * 0.5f) - 175,
      (int)(screenRect.YValue * 0.2f) + (25 * (i + 6)),
      L"Press enter to continue", MyDrawEngine::WHITE, TEXT_FONT);

    MyInputs::GetInstance()->SampleKeyboard();
    if (MyInputs::GetInstance()->NewKeyPressed(DIK_RETURN))
    {
      static const SoundIndex CHOOSE_SOUND = MySoundEngine::GetInstance()->LoadWav(
        L"Sounds/menu_choose.wav");
      MySoundEngine::GetInstance()->Play(CHOOSE_SOUND);
      Game::instance.ToMainMenu();
    }
  }

  void GUI::DrawHUD()
  {
    Rectangle2D viewPort = MyDrawEngine::GetInstance()->GetViewport();
    Vector2D bottomLeft = viewPort.GetTopLeft();
    bottomLeft.YValue -= 128.0f;

    //Draws all HUD elements scaled to fit across any sized screen large enough.
    DrawLives(bottomLeft);
    bottomLeft.XValue = viewPort.GetTopLeft().XValue + 
      ((viewPort.GetTopRight().XValue - viewPort.GetTopLeft().XValue) / 6.5f);
    DrawHealth(bottomLeft);
    bottomLeft.XValue = viewPort.GetTopLeft().XValue + 
      ((viewPort.GetTopRight().XValue - viewPort.GetTopLeft().XValue) / 6.5f * 3.0f);
    DrawFuel(bottomLeft);
    bottomLeft.XValue = viewPort.GetTopLeft().XValue + 
      ((viewPort.GetTopRight().XValue - viewPort.GetTopLeft().XValue) / 6.5f * 4.0f);
    DrawAmmo(bottomLeft);
    bottomLeft.XValue = viewPort.GetTopLeft().XValue + 
      ((viewPort.GetTopRight().XValue - viewPort.GetTopLeft().XValue) / 6.5f * 5.5f);
    DrawScore(bottomLeft);
  }

  void GUI::DrawLives(Vector2D& position)
  {
    static const PictureIndex LIFE_PICTURE = MyDrawEngine::GetInstance()->LoadPicture(
      L"Textures/life.bmp");
    //Draws an image for each life.
    for (unsigned int i = 0; i < Player::MAX_LIVES; ++i)
    {
      if (i < Game::instance.GetGameManager().GetPlayer().GetLives())
      {
        MyDrawEngine::GetInstance()->DrawAt(position + Vector2D(64.0f, 64.0f), LIFE_PICTURE);
      }
      position.XValue += 128.0f;
    }
  }

  void GUI::DrawHealth(Vector2D& position)
  {
    Rectangle2D healthRect;
    //Draws a background rectangle for health.
    healthRect.PlaceAt(position + Vector2D(0.0f, 20.0f), position + Vector2D(1020.0f, 108.0f));
    MyDrawEngine::GetInstance()->FillRect(healthRect, MyDrawEngine::GREY);
    float health = Game::instance.GetGameManager().GetPlayer().GetHealth();
    if (health > 0.0f)
    {
      //Draws a scaled rectangle representing health.
      healthRect.PlaceAt(
        position + Vector2D(20.0f, 40.0f), position + Vector2D(10.0f * health, 88.0f));
      MyDrawEngine::GetInstance()->FillRect(healthRect, MyDrawEngine::RED);
    }
  }

  void GUI::DrawFuel(Vector2D& position)
  {
    unsigned int fuelPercentage = 
      (unsigned int)((Game::instance.GetGameManager().GetPlayer().GetFuel() / 
      Player::MAX_FUEL) * 100.0f);
    uint8_t redComponent = 255;
    Rectangle2D fuelRect;
    //Draws a background rectangle for fuel.
    fuelRect.PlaceAt(
      position + Vector2D(0.0f, 20.0f), position + Vector2D(340.0f, 108.0f));
    MyDrawEngine::GetInstance()->FillRect(fuelRect, MyDrawEngine::GREY);
    position.XValue += 20.0f;
    //Draws different elements for fuel guage.
    for (unsigned int i = 0; i < 100; i += 10)
    {
      if (fuelPercentage > i)
      {
        //Draws single fuel block.
        fuelRect.PlaceAt(position + Vector2D(0.0f, 40.0f), position + Vector2D(30.0f, 88.0f));
        MyDrawEngine::GetInstance()->FillRect(
          fuelRect, D3DCOLOR_ARGB(255, redComponent, -redComponent, 50));
        redComponent -= 255 / 10;
      }
      position.XValue += 30.0f;
    }
  }

  void GUI::DrawAmmo(Vector2D& position)
  {
    Rectangle2D ammoRect;
    //Draws ammo background rectangle.
    ammoRect.PlaceAt(position + Vector2D(0.0f, 20.0f),
      position + Vector2D(Player::MAX_AMMO * 25.0f + 40.0f, 108.0f));
    MyDrawEngine::GetInstance()->FillRect(ammoRect, MyDrawEngine::GREY);
    position.XValue += 20.0f;
    //Draw a rectangle with each shot available to the player.
    for (unsigned int i = 0; i < Player::MAX_AMMO; ++i)
    {
      if (Game::instance.GetGameManager().GetPlayer().GetAmmo() > i)
      {
        ammoRect.PlaceAt(position + Vector2D(0.0f, 40.0f), position + Vector2D(20.0f, 88.0f));
        MyDrawEngine::GetInstance()->FillRect(ammoRect, MyDrawEngine::YELLOW);
      }
      position.XValue += 25.0f;
    }
  }

  void GUI::DrawScore(Vector2D& position)
  {
    static const FontIndex TEXT_FONT = MyDrawEngine::GetInstance()->AddFont(
      L"Stencil", 24, false, false);
    int highestDistanceScore =
      Game::instance.GetGameManager().GetPlayer().GetHighestDistanceScore();
    //Draw the distance score.
    std::wstring string = L"Score: " + std::to_wstring(highestDistanceScore) + L" + " + 
      std::to_wstring(Game::instance.GetGameManager().GetPlayer().GetScore() - highestDistanceScore);
    MyDrawEngine::GetInstance()->WriteText(
      position + Vector2D(0.0f, 85.0f), (wchar_t*)string.c_str(), MyDrawEngine::WHITE, TEXT_FONT);
  }
}