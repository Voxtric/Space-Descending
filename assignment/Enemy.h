#pragma once

#include "GameObject.h"
#include "EntityInfo.hpp"
#include "mysoundengine.h"

namespace Voxtric
{
  class Enemy : public GameObject
  {
  public:
    static const unsigned int TYPE_COUNT = 3; //Number of different enemy types.

    Enemy(Vector2D position, unsigned int scoreAddition, ObjectType objectType);

    virtual Circle2D& GetBoundingCircle() = 0;
    virtual std::vector<IShape2D*>& GetCollisionShapes() = 0;

    virtual void Update(float frameTime) override;

    EntityInfo GetInfo() const;

  protected:
    //Number of different explosions at any one time.
    static const unsigned int SIMULTANEOUS_EXPLOSIONS = 10; 
    static const float EXPLOSION_SCALE; //the size of all explosions.

    static SoundIndex s_explodeSounds[];
    static unsigned int s_explodeSoundIndex;

    const unsigned int m_scoreAddition;

    void Explode(float size);

  private:
    //Number of particles made by an explosion per second.
    static const unsigned int EXPLOSION_PARTICLES_PER_SECOND = 100; 
    static const float EXPLOSION_PARTICLES_LIFE;  //Duration of explosion particles.

    virtual void TryDespawn() = 0;  //Attempts to despawn the enemy for going off-screen.
  };
}