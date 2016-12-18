#include "Enemy.h"

#include "gamecode.h"
#include "Explosion.h"
#include "Projectile.h"
#include "ParticleEmitter.h"
#include "EntityInfo.hpp"

namespace Voxtric
{
  const float Enemy::EXPLOSION_SCALE = 2.0f;
  const float Enemy::EXPLOSION_PARTICLES_LIFE = 0.15f;

  SoundIndex Enemy::s_explodeSounds[Enemy::SIMULTANEOUS_EXPLOSIONS];
  unsigned int Enemy::s_explodeSoundIndex = 0;

  Enemy::Enemy(Vector2D position, unsigned int scoreAddition, ObjectType objectType) :
    m_scoreAddition(scoreAddition), GameObject(objectType)
  {
    //Loads sound in just once.
    static bool l_loadSounds = true;
    if (l_loadSounds)
    {
      for (unsigned int i = 0; i < SIMULTANEOUS_EXPLOSIONS; ++i)
      {
        s_explodeSounds[i] = MySoundEngine::GetInstance()->LoadWav(L"Sounds/big_explosion.wav");
      }
      l_loadSounds = false;
    }
    m_position = position;
  }

  void Enemy::Update(float frameTime)
  {
    TryDespawn();
  }

  EntityInfo Enemy::GetInfo() const
  {
    return EntityInfo(m_position, TYPE);
  }

  void Enemy::Explode(float size)
  {
    //Explodes the enemy.
    Deactivate();
    MySoundEngine::GetInstance()->Play(s_explodeSounds[s_explodeSoundIndex]);
    ++s_explodeSoundIndex;
    if (s_explodeSoundIndex == SIMULTANEOUS_EXPLOSIONS)
    {
      s_explodeSoundIndex = 0;
    }
    //Creates a basic explosion.
    Game::instance.m_objects.AddItem(new Explosion(m_position, size, EXPLOSION_SCALE), true);
    Game::instance.m_objects.AddItem(new ParticleEmitter(
      m_position, EXPLOSION_PARTICLES_LIFE, EXPLOSION_PARTICLES_PER_SECOND, 
      MyDrawEngine::YELLOW, (int)(size * 150.0f)), false);
  }
}