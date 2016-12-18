#include "ParticleEmitter.h"

#include "Player.h"

#define TO_RAD * 3.14f / 180.0f //I don't like radians or accurate PI measurements.

namespace Voxtric
{
  const float ParticleEmitter::MAX_PARTICLE_LIFE = 1.5f;
  const float ParticleEmitter::PARTICLE_GRAVITY = 1000.0f;
  const float ParticleEmitter::PARTICLE_FRICTION = 1.5f;

  ParticleEmitter::ParticleEmitter(Vector2D position, float duration, 
    unsigned int particlesPerFrame, unsigned int colour, int force) :
    GameObject(ObjectType::UNKNOWN)
  {
    //Initialises basic values.
    m_position = position;
    m_totalDuration = duration;
    m_lifeRemaining = duration;
    m_particlesPerFrame = particlesPerFrame;
    m_colour = colour;
    m_force = force;
    m_collisionShapes.emplace_back(&m_boundingCircle);
    m_drawDepth = 100;
    m_collisionShapes.push_back(&m_boundingCircle);
  }

  //Struct initialiser.
  ParticleEmitter::Particle::Particle(unsigned int colour, float duration, Vector2D position,
    Vector2D velocity)
  {
    this->colour = colour;
    this->lifeRemaining = duration;
    this->position = position;
    this->velocity = velocity;
  }

  void ParticleEmitter::SpawnParticles()
  {
    //Spawns the number of particles required in random directions.
    for (unsigned int i = 0; i < m_particlesPerFrame; ++i)
    {
      Vector2D velocity;
      velocity.setBearing(
        (float)(rand() % PARTICLE_SPREAD_ANGLE) TO_RAD, (float)(rand() % m_force));
      m_particles.emplace_back(m_colour, MAX_PARTICLE_LIFE, m_position, velocity);
    }
  }

  void ParticleEmitter::UpdateParticles(float frameTime)
  {
    //Simulates the basic movement of all particles.
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
      if (m_particles[i].lifeRemaining > 0.0f)
      {
        m_particles[i].velocity.YValue -= PARTICLE_GRAVITY * frameTime;
        m_particles[i].velocity -= PARTICLE_FRICTION * frameTime * m_particles[i].velocity;
        m_particles[i].position += m_particles[i].velocity * frameTime;
        m_particles[i].lifeRemaining -= frameTime;
      }
      else
      {
        //Deletes the dead particle.
        m_particles[i] = m_particles[m_particles.size() - 1];
        m_particles.pop_back();
        --i;
      }
    }
  }

  void ParticleEmitter::Update(float frameTime)
  {
    //Updates all particles.
    if (m_lifeRemaining > 0.0f)
    {
      SpawnParticles();
      m_lifeRemaining -= frameTime;
    }
    UpdateParticles(frameTime);
    if (m_particles.size() == 0)
    {
      Deactivate();
    }
  }

  void ParticleEmitter::ProcessCollision(GameObject& other) {}

  Circle2D& ParticleEmitter::GetBoundingCircle()
  {
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& ParticleEmitter::GetCollisionShapes()
  {
    return m_collisionShapes;
  }

  void ParticleEmitter::Draw()
  {
    if (m_particles.size() > 0)
    {
      //Collects all particle positions and colours together so that they can be drawn in a single
      //draw call.
      m_positions.resize(m_particles.size());
      m_colours.resize(m_particles.size());
      for (unsigned int i = 0; i < m_particles.size(); ++i)
      {
        m_positions[i] = m_particles[i].position;
        m_colours[i] = m_particles[i].colour;
      }
      MyDrawEngine::GetInstance()->DrawPointList(
        &m_positions[0], &m_colours[0], m_particles.size());
    }
  }
}