#include "Plane.h"

#include "gamecode.h"
#include "Explosion.h"
#include "Projectile.h"

namespace Voxtric
{
  const float Plane::SPEED = 600.0f;

  SoundIndex Plane::s_engineSound = 0;
  unsigned int Plane::s_instances = 0;
  bool Plane::s_engineSoundPlaying = false;

  Plane::Plane(Vector2D position) : Enemy(position, SCORE_MODIFIER, ObjectType::PLANE)
  {
    LoadImage(L"Textures/plane.bmp");
    m_collisionShapes.emplace_back(&m_collisionShape);

    //Loads the sound only once.
    if (s_engineSound == 0)
    {
      s_engineSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/plane_engine.wav");
    }
    ++s_instances;
  }

  void Plane::TryDespawn()
  {
    //Despawns the plane if it goes off the screen.
    if (m_position.XValue + GameManager::DESPAWN_BUFFER_DISTANCE < 
      MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue && 
      m_position.XValue + GameManager::DESPAWN_BUFFER_DISTANCE < 
      Game::instance.GetGameManager().GetPlayerSpawnPosition().XValue)
    {
      Plane::TryStopSound();
      Deactivate();
    }
  }

  void Plane::Update(float frameTime)
  {
    Enemy::Update(frameTime);
    //Moves the plane across the screen.
    m_position.XValue -= SPEED * frameTime;
    if (s_instances > 0 && !s_engineSoundPlaying)
    {
      MySoundEngine::GetInstance()->Play(s_engineSound, true);
      s_engineSoundPlaying = true;
    }
  }

  void Plane::OnPause()
  {
    //Stops playing the sound during the pause menu.
    if (s_engineSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_engineSound);
      s_engineSoundPlaying = false;
    }
  }

  void Plane::TryStopSound()
  {
    //Only stops the plane sounds when there are no more planes on screen.
    --s_instances;
    if (s_instances == 0)
    {
      s_engineSoundPlaying = false;
      MySoundEngine::GetInstance()->Stop(s_engineSound);
    }
  }

  void Plane::ProcessCollision(GameObject& other)
  {
    //Destroys the ship.
    if (other.GetType() == ObjectType::PROJECTILE)
    {
      if (((Projectile*)&other)->FiredByPlayer())
      {
        other.Deactivate();
        Explode(3.0f);
        Game::instance.GetGameManager().GetPlayer().AddScore(m_scoreAddition);
        Plane::TryStopSound();
      }
    }
    else if (other.GetType() == ObjectType::PLAYER)
    {
      Deactivate();
      Plane::TryStopSound();
    }
    else if (other.GetType() == ObjectType::GRINDER)
    {
      Explode(3.0f);
      Plane::TryStopSound();
    }
  }

  Circle2D& Plane::GetBoundingCircle()
  {
    //Plane width (different for collision circles)
    m_boundingCircle.PlaceAt(m_position, 226.0f * m_imageScale);
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Plane::GetCollisionShapes()
  {
    //Plane width and plane heights (different for bounding circle)
    m_collisionShape.PlaceAt(
      m_position - Vector2D(88.0f, 13.0f) * m_imageScale, 
      m_position + Vector2D(88.0f, 13.0f) * m_imageScale);   
    return m_collisionShapes;
  }
}