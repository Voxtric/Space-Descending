#pragma once

#include "Pickup.h"

namespace Voxtric
{
  class SpeedBoostPickup : public Pickup
  {
  public:
    SpeedBoostPickup(Vector2D position) : Pickup(position, PickupType::Speed)
    {
      LoadImage(L"Textures/speed_boost_crate.bmp");
    }

    void Activate() override
    {
      ACTIVE_PLAYER.BoostSpeed();
    }
  };

  class HealthBoostPickup : public Pickup
  {
  public:
    HealthBoostPickup(Vector2D position) : Pickup(position, PickupType::Health)
    {
      LoadImage(L"Textures/health_boost_crate.bmp");
    }

    void Activate() override
    {
      ACTIVE_PLAYER.BoostHealth();
    }
  };

  class LifeBoostPickup : public Pickup
  {
  public:
    LifeBoostPickup(Vector2D position) : Pickup(position, PickupType::Life)
    {
      LoadImage(L"Textures/life_boost_crate.bmp");
    }

    void Activate() override
    {
      ACTIVE_PLAYER.BoostLives();
    }
  };

  class ScoreBoostPickup : public Pickup
  {
  public:
    static const unsigned int MAXIMUM_SCORE_INCREASE = 10000; //Maximum player score increase.

    ScoreBoostPickup(Vector2D position) : Pickup(position, PickupType::Score)
    {
      LoadImage(L"Textures/score_boost_crate.bmp");
    }

    void Activate() override
    {
      ACTIVE_PLAYER.AddScore(MAXIMUM_SCORE_INCREASE);
    }
  };

  class RandomBoostPickup : public Pickup
  {
  private:
    static const unsigned int EFFECT_COUNT = 6;

  public:
    RandomBoostPickup(Vector2D position) : Pickup(position, PickupType::Random)
    {
      LoadImage(L"Textures/random_boost_crate.bmp");
    }

    void Activate() override
    {
      switch (rand() % EFFECT_COUNT)
      {
      case 0:
        ACTIVE_PLAYER.BoostSpeed();
        break;
      case 1:
        if (ACTIVE_PLAYER.GetHealth() < Player::MAX_HEALTH)
        {
          ACTIVE_PLAYER.BoostHealth();
          break;
        }
      case 2:
        if (ACTIVE_PLAYER.GetLives() < Player::MAX_LIVES)
        {
          ACTIVE_PLAYER.BoostLives();
          break;
        }
      case 3:
        ACTIVE_PLAYER.AddScore(rand() % ScoreBoostPickup::MAXIMUM_SCORE_INCREASE);
        break;
      case 4:
        ACTIVE_PLAYER.InvertControls();
        break;
      case 5:
        Game::instance.GetGameManager().RandomiseGravity();
        break;
      }
    }
  };
}