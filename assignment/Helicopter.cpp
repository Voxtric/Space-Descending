#include "Helicopter.h"

#include "Explosion.h"
#include "gamecode.h"
#include "Projectile.h"

namespace Voxtric
{
  const float Helicopter::WAIT_PER_PLAYER_CHASE = 1.0f;
  const float Helicopter::CHASE_SPEED = 1000.0f;

  SoundIndex Helicopter::s_engineSound = 0;
  unsigned int Helicopter::s_instances = 0;
  bool Helicopter::s_engineSoundPlaying = false;

  Helicopter::Helicopter(Vector2D position) :
    Enemy(position, SCORE_MODIFIER, ObjectType::HELICOPTER)
	{
    //Random period of time less than a second.
    m_timeSincePlayerChase = (float)(rand() % PLAYER_CHASE_WAIT_MILISECONDS) / 
      (float)PLAYER_CHASE_WAIT_MILISECONDS;
    m_targetYPos = 0.0f;
    m_collisionShapes.emplace_back(&m_collisionShape);

    //Loads all the images for the animation.
    for (unsigned int i = 0; i < FRAME_COUNT; ++i)
    {
      std::wstring texturePath = L"Textures/helicopter/helicopter_" + std::to_wstring(i) + L".bmp";
      LoadImage((wchar_t*)texturePath.c_str());
    }

    if (s_engineSound == 0)
    {
      s_engineSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/helicopter_engine.wav");
    }
    ++s_instances;
	}

  void Helicopter::ProcessCollision(GameObject& other)
  {
    if (other.GetType() == ObjectType::PROJECTILE)
    {
      //Destroys the helicopter and removes it from the list of spawnable buildings.
      if (((Projectile*)&other)->FiredByPlayer())
      {
        other.Deactivate();
        Explode(EXPLOSION_SCALE);
        Game::instance.GetGameManager().GetPlayer().AddScore(m_scoreAddition);
        Game::instance.GetGameManager().RemoveEntity(m_position.XValue, true);
        Helicopter::TryStopSound();
      }
    }
    //Explodes the helicopter when it flies into a building.
    else if (other.GetType() == ObjectType::BUILDING)
    {
      Explode(EXPLOSION_SCALE);
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, true);
      Helicopter::TryStopSound();
    }
    else if (other.GetType() == ObjectType::PLAYER)
    {
      Deactivate();
      Helicopter::TryStopSound();
    }
    else if (other.GetType() == ObjectType::GRINDER)
    {
      Explode(3.0f);
      Helicopter::TryStopSound();
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, true);
    }
  }

  void Helicopter::TryStopSound()
  {
    //Stops playing helicopter sounds if there are no more on screen.
    --s_instances;
    if (s_instances == 0)
    {
      s_engineSoundPlaying = false;
      MySoundEngine::GetInstance()->Stop(s_engineSound);
    }
  }

  void Helicopter::Update(float frameTime)
  {
    Enemy::Update(frameTime);
    
    //Sets the target position to chase the player once every period of time.
    m_imageNumber = m_imageNumber == FRAME_COUNT - 1 ? 0 : m_imageNumber + 1;
    m_timeSincePlayerChase += frameTime;
    if (m_timeSincePlayerChase >= WAIT_PER_PLAYER_CHASE)
    {
      m_timeSincePlayerChase = 0.0f;
      m_targetYPos = Game::instance.GetGameManager().GetPlayer().GetPosition().YValue;
    }

    //Moves the helicopter towards its target position.
    if (m_targetYPos != 0.0f)
    {
      if (m_targetYPos < m_position.YValue)
      {
        m_position.YValue -= CHASE_SPEED * frameTime;
        if (m_targetYPos > m_position.YValue)
        {
          m_targetYPos = 0.0f;
        }
      }
      else
      {
        m_position.YValue += CHASE_SPEED * frameTime;
        if (m_targetYPos < m_position.YValue)
        {
          m_targetYPos = 0.0f;
        }
      }
    }
    if (s_instances > 0 && !s_engineSoundPlaying)
    {
      MySoundEngine::GetInstance()->Play(s_engineSound, true);
      s_engineSoundPlaying = true;
    }
  }
  
  void Helicopter::TryDespawn()
  {
    //Despawns the helicopter if it goes too far off the screen.
    if (m_position.XValue + GameManager::DESPAWN_BUFFER_DISTANCE <
      MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue || 
      m_position.XValue - GameManager::DESPAWN_BUFFER_DISTANCE > 
      MyDrawEngine::GetInstance()->GetViewport().GetBottomRight().XValue)
    {
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, false);
      Helicopter::TryStopSound();
      Deactivate();
    }
  }

  Circle2D& Helicopter::GetBoundingCircle()
  {
    //Helicopter width (different to collision shapes)
    m_boundingCircle.PlaceAt(m_position, 386.0f);   
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Helicopter::GetCollisionShapes()
  {
    //Helicopter width and height (different to bounding circle)
    m_collisionShape.PlaceAt(
      m_position - Vector2D(68.0f, 20.0f), 
      m_position + Vector2D(83.0f, 10.0f));   
    return m_collisionShapes;
  }

  void Helicopter::OnPause()
  {
    if (s_engineSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_engineSound);
      s_engineSoundPlaying = false;
    }
  }
}