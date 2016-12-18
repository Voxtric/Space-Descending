#pragma once

#include "GameObject.h"

namespace Voxtric
{
  class ParticleEmitter : public GameObject
  {
  private:
    static const float MAX_PARTICLE_LIFE; //Maximum amount of time a particle can exist.
    static const float PARTICLE_GRAVITY;  //Level of gravity to affect all particles.
    static const float PARTICLE_FRICTION; //Level of friction to affect all particles.
    //Different angles that particles can be spread at.
    static const unsigned int PARTICLE_SPREAD_ANGLE = 360; 

    //Only a particle emitter should ever be aware of individual particles.
    struct Particle
    {
    public:
      unsigned int colour;
      float lifeRemaining;
      Vector2D position;
      Vector2D velocity;

      Particle(unsigned int colour, float duration, Vector2D position, Vector2D velocity);
    };

    float m_totalDuration;
    float m_lifeRemaining;
    unsigned int m_particlesPerFrame;
    unsigned int m_colour;
    int m_force;

    std::vector<Particle> m_particles;
    std::vector<Vector2D> m_positions;
    std::vector<unsigned int> m_colours;

    void SpawnParticles();  //Spawns all particles necessary.
    void UpdateParticles(float frameTime);  //Updates the velocity and position of all particles.

  public:
    ParticleEmitter(Vector2D position, float duration, unsigned int particlesPerSecond, 
      unsigned int colour, int force);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    void Draw() override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;
  };
}