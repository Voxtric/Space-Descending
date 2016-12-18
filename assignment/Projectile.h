#pragma once

#include "GameObject.h"
#include "mysoundengine.h"

namespace Voxtric
{
  class Projectile : public GameObject
  {
  public:
    Projectile(Vector2D position, float angle, float speed, bool playerFired, 
      float waitForBuildingCollide = 0.0f);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

    bool FiredByPlayer() const;

  private:
    static const unsigned int FRAME_COUNT = 12; //Frames in the animation.
    static const unsigned int DRAW_DEPTH = 11;  
    //How many sounds that can be played at once.
    static const unsigned int SIMULTANEOUS_SOUNDS = 10;
    static const unsigned int PARTICLES_PER_SECOND = 3; //Particles that can be produced per second.
    //Velocity with which to throw particles.
    static const unsigned int PARTICLES_EXPULSION_FORCE = 100; 
    static const float PROJECTILE_RADIUS; //Radius of the projectile.
    static const float IMAGE_SCALE; //Scale of the projectile.
    static const float EXPLOSION_SCALE; //Size of the explosion to create.
    static const float EXPLOSION_SPEED; //The speed with which to play the explosion animation.
    static const float EXPLOSION_DURATION;  //The duration that the explosion should last.
    static const float DAMAGE_DEALT; //Damage dealt to the player on collision.

    static SoundIndex s_shootSounds[SIMULTANEOUS_SOUNDS];
    static unsigned int s_shootSoundIndex;
    static SoundIndex s_explodeSounds[SIMULTANEOUS_SOUNDS];
    static unsigned int s_explodeSoundIndex;

    Vector2D m_velocity;
    int m_animationDirection;
    bool m_playerFired;
    float m_waitForBuildingCollide;
  };
}