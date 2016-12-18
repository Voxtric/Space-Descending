#pragma once

#include "GameObject.h"

namespace Voxtric
{
  class Explosion : public GameObject
  {
  public:
    Explosion(Vector2D position, float scale, float loopsPerSecond, unsigned int startFrame = 0);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

  private:
    static const unsigned int FRAME_COUNT = 12; //Images making up the animation.
    static const float EXPLOSION_FORCE; //Force with which to propel the player.

    float m_frame;
    float m_loopsPerSecond;
  };
}