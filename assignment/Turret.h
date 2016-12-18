#pragma once

#include "Enemy.h"

namespace Voxtric
{
  class Turret : public Enemy
  {
  public:
    Turret(Vector2D position);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

  private:
    static const unsigned int SCORE_MODIFIER = 5000; //Amount to increase score by on death.
    static const unsigned int PLAYER_ATTACK_WAIT_MILLISECONDS = 1000; //Wait per attack on player.
    static const float DELAY_PER_SHOT;  //Delay between each shot.
    static const float PROJECTILE_SHOOT_SPEED;  //Speed of the projectiles shot.
    static const float QUARTER_TURN_ANGLE;  //90 degrees in radians.
    static const float RADIUS;  //Radius of the turret sprite.

    float m_timeSinceLastShot;

    void TryDespawn() override;
  };
}