#pragma once

#include "Enemy.h"

namespace Voxtric
{
  class Plane : public Enemy
  {
  public:
    Plane(Vector2D position);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    void OnPause() override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;
  
  private:
    static const unsigned int SCORE_MODIFIER = 1000;  //Score increase when destroyed.
    static const float SPEED;   //Speed travelled at. 

    static SoundIndex s_engineSound;
    static unsigned int s_instances;
    static bool s_engineSoundPlaying;

    static void TryStopSound();

    Rectangle2D m_collisionShape;

    void TryDespawn() override;
  };
}