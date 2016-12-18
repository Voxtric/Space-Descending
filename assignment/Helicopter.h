#pragma once

#include "Enemy.h"

namespace Voxtric
{
	class Helicopter : public Enemy
  {
  public:
    Helicopter(Vector2D position);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    void OnPause() override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

  private:
    static const unsigned int SCORE_MODIFIER = 1200;  //Amount to increase score by on death.
    //Delay between each movement to chase player.
    static const unsigned int PLAYER_CHASE_WAIT_MILISECONDS = 1000; 
    static const unsigned int FRAME_COUNT = 4;  //Number of images making up the animation.
    static const float WAIT_PER_PLAYER_CHASE;
    static const float CHASE_SPEED; //Speed to chase the player at.

    static SoundIndex s_engineSound;
    static unsigned int s_instances;
    static bool s_engineSoundPlaying;

    static void TryStopSound();

    Rectangle2D m_collisionShape;
    float m_timeSincePlayerChase;
    float m_targetYPos;

    void TryDespawn() override;
	};
}