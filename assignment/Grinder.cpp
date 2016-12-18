#include "Grinder.h"

#include "gamecode.h"
#include "ParticleEmitter.h"

namespace Voxtric
{
  const float Grinder::WAIT_PER_MOVE = 1.0f;
  const float Grinder::MOVE_SPEED = 2000.0f;
  const float Grinder::PLAYER_DAMAGE = 0.5f;
  const float Grinder::RADIUS = 64.0f;

  SoundIndex Grinder::s_grindSounds[SIMULTANEOUS_SOUNDS];
  unsigned int Grinder::s_grindSoundIndex = 0;
  SoundIndex Grinder::s_backgroundSound = 0;
  unsigned int Grinder::s_instances = 0;
  bool Grinder::s_backgroundSoundPlaying = false;

  Grinder::Grinder(Vector2D position) : Enemy(position, SCORE_MODIFIER, ObjectType::GRINDER)
  {
    //Loads all sounds that grinders use.
    static bool l_loadSounds = true;
    if (l_loadSounds)
    {
      for (unsigned int i = 0; i < SIMULTANEOUS_SOUNDS; ++i)
      {
        s_grindSounds[i] = MySoundEngine::GetInstance()->LoadWav(L"Sounds/grinder_grind.wav");
      }
      s_backgroundSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/grinder_background.wav");
      l_loadSounds = false;
    }

    //Time until player chase.
    m_timeSincePlayerChase = (float)(rand() % PLAYER_CHASE_WAIT_MILISECONDS) /
      (float)PLAYER_CHASE_WAIT_MILISECONDS;
    m_targetPosition = GetNewTargetPosition();
    m_spin = rand() % 2 == 0 ? 4.0f: -4.0f; //Random direction to spin the ship.
    m_activeSoundIndex = NO_ACTIVE_SOUND;
    m_collisionShapes.emplace_back(&m_boundingCircle); 
    LoadImage(L"Textures/grinder.bmp");

    ++s_instances;
  }

  void Grinder::Update(float frameTime)
  {
    //Spins the ship.
    m_angle += m_spin * frameTime;

    if (m_position == m_targetPosition)
    {
      //Waits at the target position for a while before moving on.
      m_timeSincePlayerChase += frameTime;
      if (m_timeSincePlayerChase > WAIT_PER_MOVE)
      {
        m_targetPosition = GetNewTargetPosition();
        m_timeSincePlayerChase = 0.0f;
      }
    }
    //Moves the ship towards the target position.
    else
    {
      //Only moves the full distance if it won't overshoot the target position.
      if ((m_targetPosition - m_position).magnitude() > MOVE_SPEED * frameTime)
      {
        Vector2D movement;
        movement.setBearing((m_targetPosition - m_position).angle(), MOVE_SPEED);
        m_position += movement * frameTime;
      }
      else
      {
        m_position = m_targetPosition;
      }
    }

    //Checks if the grind sound should stop playing.
    Vector2D playerPos = Game::instance.GetGameManager().GetPlayer().GetPosition();
    if ((playerPos - m_position).magnitude() > RADIUS * 2.0f && 
      m_activeSoundIndex != NO_ACTIVE_SOUND)
    {
      MySoundEngine::GetInstance()->Stop(s_grindSounds[m_activeSoundIndex]);
      m_activeSoundIndex = NO_ACTIVE_SOUND;
    }

    //Checks if the background sound should stop playing.
    if (s_instances > 0 && !s_backgroundSoundPlaying)
    {
      MySoundEngine::GetInstance()->Play(s_backgroundSound, true);
      s_backgroundSoundPlaying = true;
    }
  }

  void Grinder::ProcessCollision(GameObject& other)
  {
    if (other.GetType() == ObjectType::PLAYER)
    {
      //Damages the player.
      ((Player*)&other)->Damage(PLAYER_DAMAGE);
      //Get the collidiong shape.
      std::vector<IShape2D*> collisionShapes = ((Player*)&other)->GetCollisionShapes();            
      for (unsigned int i = 0; i < collisionShapes.size(); ++i)
      {
        if (collisionShapes[i]->Intersects(GetBoundingCircle()))
        {
          //Create particles at the collision position.
          Vector2D collisionPosition = 
            ((Segment2D*)collisionShapes[i])->Intersection(m_boundingCircle);
          ParticleEmitter* particleEmitter =
            new ParticleEmitter(collisionPosition, 0.1f, 10, MyDrawEngine::YELLOW, 300);
          Game::instance.m_objects.AddItem(particleEmitter, false);

          //Starts playing the grinding sound.
          if (m_activeSoundIndex == NO_ACTIVE_SOUND)
          {
            m_activeSoundIndex = s_grindSoundIndex;
            ++s_grindSoundIndex;
            if (s_grindSoundIndex == SIMULTANEOUS_SOUNDS)
            {
              s_grindSoundIndex = 0;
            }
            MySoundEngine::GetInstance()->Play(s_grindSounds[m_activeSoundIndex], true);
          }
          break;
        }
      }
    }
    //Kills the grinder.
    else if (other.GetType() == ObjectType::PROJECTILE)
    {
      other.Deactivate();
      Explode(EXPLOSION_SCALE);
      Game::instance.GetGameManager().GetPlayer().AddScore(m_scoreAddition);
      //Disables the sounds where necessary.
      if (m_activeSoundIndex != NO_ACTIVE_SOUND)
      {
        MySoundEngine::GetInstance()->Stop(s_grindSounds[m_activeSoundIndex]);
        m_activeSoundIndex = NO_ACTIVE_SOUND;
      }
      --s_instances;
      if (s_instances == 0)
      {
        s_backgroundSoundPlaying = false;
        MySoundEngine::GetInstance()->Stop(s_backgroundSound);
      }
    }
  }

  void Grinder::OnPause()
  {
    //Disables all sounds.
    if (m_activeSoundIndex != NO_ACTIVE_SOUND)
    {
      MySoundEngine::GetInstance()->Stop(s_grindSounds[m_activeSoundIndex]);
      m_activeSoundIndex = NO_ACTIVE_SOUND;
    }
    if (s_backgroundSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_backgroundSound);
      s_backgroundSoundPlaying = false;
    }
  }

  Circle2D& Grinder::GetBoundingCircle()
  {
    m_boundingCircle.PlaceAt(m_position, RADIUS);
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Grinder::GetCollisionShapes()
  {
    return m_collisionShapes;
  }

  bool Grinder::PathClear(const Segment2D& path) const
  {
    bool pathClear = true;
    //Finds if there are any collision objects that should obstruct the Grinders path.
    std::vector<GameObject*> collisionObjects;
    if (Game::instance.m_objects.ShapeCollides(&path, collisionObjects))
    {
      for (unsigned int i = 0; i < collisionObjects.size(); ++i)
      {
        if (collisionObjects[i]->GetType() != ObjectType::GRINDER)
        {
          pathClear = false;
        }
      }
    }
    return pathClear;
  }

  Vector2D Grinder::GetNewTargetPosition() const
  {
    Vector2D position = m_position;
    bool positionValid = true;
    unsigned int attempts = 0;
    if (Game::instance.GetGameManager().GetPlayer().GetPosition().YValue > 0.0f)
    {
      do
      {
        positionValid = true;
        //Gets a random position around the player.
        float randomDistance = (float)(rand() % MAX_DISTANCE_FROM_PLAYER);
        float randomAngle = (float)(rand() % 360) * 3.14f / 180.0f;
        position.setBearing(randomAngle, randomDistance);
        position += Game::instance.GetGameManager().GetPlayer().GetPosition();

        //Tests if the position chosen would collide with another object.
        std::vector<GameObject*> collisionObjects;
        Circle2D collisionTest;
        collisionTest.PlaceAt(position, RADIUS);
        positionValid = !Game::instance.m_objects.ShapeCollides(&collisionTest, collisionObjects);   
        //Tests if the position chosen can be reached by the grinder.
        if (positionValid)
        {
          Segment2D pathTest;
          pathTest.PlaceAt(m_position + Vector2D(0, RADIUS).rotatedBy(-m_angle),
            position + Vector2D(0, RADIUS).rotatedBy(-(position - m_position).angle()));
          positionValid = PathClear(pathTest);
          if (positionValid)
          {
            pathTest.PlaceAt(m_position - Vector2D(0, RADIUS).rotatedBy(-m_angle), 
              position - Vector2D(0, RADIUS).rotatedBy(-(position - m_position).angle()));
            positionValid = PathClear(pathTest);
          }
        }
        ++attempts;
      } while ((!positionValid || position.YValue < 0.0f) && attempts < MAX_NEW_POSITION_ATTEMPTS);
    }

    //Sets the target position to the current position if a valid position could not be determined.
    if (attempts == MAX_NEW_POSITION_ATTEMPTS)
    {
      position = m_position;
    }
    return position;
  }
  
  void Grinder::TryDespawn() {}
}