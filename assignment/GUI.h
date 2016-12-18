#pragma once

#include "vector2D.h"

namespace Voxtric
{
  class GUI
  {
  public:
    void DrawMenuBackground();  //Draws background of most menus.
    void DrawTiledBackground(bool useParalax); //Draws background of game.
    void DrawHUD();         

    void DrawMainMenu(int menuOption);  
    void DrawPauseMenu(int menuOption); 
    void DrawScoreMenu();      
    void DrawHowToPlay();

  private:
    static const unsigned int BACKGROUND_BUILDINGS = 30;  //Number of building drawn in a menu.
    static const unsigned int MAIN_TITLE_FONT_SIZE = 120; //Game title font size.
    static const unsigned int TITLE_FONT_SIZE = 90;       //Game sub title font size.
    static const unsigned int TEXT_FONT_SIZE = 60;        //Game general text font size.
    static const unsigned int SMALL_TEXT_FONT_SIZE = 30;  //Game small text font size.
    static const float BACKGROUND_TILE_SIZE;              //Background image tile dimensions.

    void DrawLives(Vector2D& position);   
    void DrawHealth(Vector2D& position);  
    void DrawFuel(Vector2D& position);
    void DrawAmmo(Vector2D& position);
    void DrawScore(Vector2D& position);
  };
}