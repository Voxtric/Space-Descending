#include "Player.h"

#include "gamecode.h"
#include "myinputs.h"
#include "Explosion.h"
#include "Projectile.h"
#include "ParticleEmitter.h"
#include "mysoundengine.h"

namespace Voxtric
{
  //Public player constants
  const float Player::MAX_HEALTH = 100.0f;
  const float Player::MAX_FUEL = 25.0f;
  const float Player::FRICTION = 1.5f;

  //Private player constants
  const float Player::COLLISION_THRESHOLD = 250.0f;
  const float Player::COLLIDED_TURN_WAIT = 0.25f;

  const float Player::PLAYER_RESET_YVALUE = -200.0f;
  const float Player::PLAYER_RESET_WAIT = 2.0f;
  const float Player::LOW_HEALTH_THRESHOLD = 40.0f;

  const float Player::ACCELERATION = 1000.0f;
  const float Player::TURN_SPEED = 3.0f;
  const float Player::NO_TARGET = -1.0f;

  const float Player::HEALTH_REDUCTION_MODIFIER = 0.025f;
  const float Player::FUEL_INCREMENT = 1.5f;

  const float Player::BOOST_DURATION = 10.0f;
  const float Player::BOOST_SPEED_MODIFIER = 2000.0f;

  const float Player::WAIT_TIME_PER_SHOT = 0.15f;
  const float Player::PROJECTILE_SHOOT_SPEED = 2000.0f;

  const float Player::CAMERA_BACK_CONSTRAINT = 2000.0f;
  const float Player::CAMERA_UP_CONSTRAINT = 1400.0f;
  const float Player::CAMERA_DOWN_CONSTRAINT = 1000.0f;
  const float Player::PLAYER_BACK_CONSTRAINT = 1000.0f;

  const float Player::EXPLOSION_SCALE = 4.0f;
  const float Player::EXPLOSION_SPEED = 3.0f;
  const float Player::EXPLOSION_PARTICLES_DURATION = 0.15f;
  const float Player::THRUST_BOTTOM = -70.0f;
  const float Player::THRUST_SIDE = 24.0f;
  const float Player::THRUSTER_SCALE = 0.25f;
  const float Player::THRUSTER_SPEED = 5.0f;
  const float Player::THRUST_VOLUME_MODIFIER = 3.0f;
  const float Player::THRUST_FUEL_COST = 0.5f;

  const float Player::LEFT_TILT_ANGLE = 0.436332f;
  const float Player::RIGHT_TILT_ANGLE = 5.84685f;
  const float Player::QUARTER_TURN_ANGLE = 1.5708f;

  SoundIndex Player::s_buildingCollisionSounds[Player::SIMULTANEOUS_SOUNDS];
  unsigned int Player::s_buildingCollisionSoundIndex = 0;
  SoundIndex Player::s_engineSound = 0;
  SoundIndex Player::s_healthLowSound = 0;

	Player::Player(Vector2D position) : GameObject(ObjectType::PLAYER)
  {
    //Load the sounds needed only once.
    if (s_engineSound == 0)
    {
      for (unsigned int i = 0; i < SIMULTANEOUS_SOUNDS; ++i)
      {
        s_buildingCollisionSounds[i] = MySoundEngine::GetInstance()->LoadWav(
          L"Sounds/player_building_collision.wav");
      }
      s_engineSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/player_thrust.wav");
      s_healthLowSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/player_health_low.wav");
    }

    //Initialises basic values.
    m_originalDistanceToTarget = 0.0f;
    m_additionalScore = 0;
    m_position = position;
    m_framesElapsed = 0;
    m_targetXPosition = 0.0f;
    m_ammo = MAX_AMMO;
    LoadImage(L"Textures/space_ship.bmp");
    m_dimensions.set(78.0f, 160.0f);
    m_health = MAX_HEALTH;
    m_timeSinceCollision = 10.0f;
    m_timeSinceLastShot = WAIT_TIME_PER_SHOT;
    m_fuel = MAX_FUEL;
	  m_targetAngle = NO_TARGET;
    m_timeUntilReset = 0.0f;
    m_needsReset = false;
    m_lives = MAX_LIVES;
    m_engineSoundPlaying = false;
    m_healthLowSoundPlaying = false;
    m_highestScore = 0;

    //Places all the collision segments making up the compound shape.
    for (unsigned int i = 0; i < COLLISION_SEGMENTS_COUNT; ++i)
    {
      m_collisionShapes.emplace_back(&m_collisionSegments[i]);
    }

    m_boostDuration = 0.0f;
    m_extraAcceleration = 0.0f;
    m_controlsInverted = false;
  }

  void Player::PlaceAt(Vector2D position)
  {
    m_position = position;
  }

  void Player::OnPause()
  {
    //Stops different sounds when the game is paused.
    if (m_engineSoundPlaying)
    {
      m_engineSoundPlaying = false;
      MySoundEngine::GetInstance()->Stop(s_engineSound);
    }
    if (m_healthLowSoundPlaying)
    {
      m_healthLowSoundPlaying = false;
      MySoundEngine::GetInstance()->Stop(s_healthLowSound);
    }
  }

  void Player::BoostSpeed()
  {
    m_extraAcceleration = BOOST_SPEED_MODIFIER;
    m_boostDuration = BOOST_DURATION;
  }

  void Player::BoostHealth()
  {
    m_health = MAX_HEALTH;
  }

  bool Player::CanRefuel() const
  {
    return m_ammo < MAX_AMMO || m_fuel < MAX_FUEL;
  }

  void Player::BoostLives()
  {
    if (m_lives < MAX_LIVES)
    {
      ++m_lives;
    }
  }

  int Player::GetScore() const
  {
    return m_highestScore + m_additionalScore;
  }

  void Player::AddScore(unsigned int score)
  {
    m_additionalScore += score;
  }

  Vector2D Player::GetPosition() const
  {
    return m_position;
  }

  void Player::Refuel()
  {
    static const SoundIndex REFUEL_SOUND = MySoundEngine::GetInstance()->LoadWav(
      L"Sounds/player_refuel.wav");
    if (m_fuel < MAX_FUEL)
    {
      m_fuel += FUEL_INCREMENT;
    }
    else
    {
      m_fuel = MAX_FUEL;
    }
    if (m_ammo < MAX_AMMO)
    {
      ++m_ammo;
    }
  }

  void Player::FindCollisionObjects(GameObject& other, 
    Segment2D*& playerColShape, Rectangle2D*& buildingColShape)
  {
    std::vector<IShape2D*> collisionShapes = GetCollisionShapes();
    std::vector<IShape2D*> otherCollisionShapes = other.GetCollisionShapes();
    //Compares all the different collision shapes of both objects to find the colliding pair.
    for (unsigned int i = 0; i < collisionShapes.size() &&
      playerColShape == nullptr; ++i)
    {
      for (unsigned int j = 0; j < otherCollisionShapes.size() &&
        buildingColShape == nullptr; ++j)
      {
        if (collisionShapes[i]->Intersects(*otherCollisionShapes[j]))
        {
          playerColShape = static_cast<Segment2D*>(collisionShapes[i]);
          buildingColShape = static_cast<Rectangle2D*>(otherCollisionShapes[j]);
        }
      }
    }
  }

  void Player::DamageShip(const Segment2D* playerColShape, const Rectangle2D* buildingColShape)
  {
    MySoundEngine::GetInstance()->Play(s_buildingCollisionSounds[s_buildingCollisionSoundIndex]);
    ++s_buildingCollisionSoundIndex;
    if (s_buildingCollisionSoundIndex == SIMULTANEOUS_SOUNDS)
    {
      s_buildingCollisionSoundIndex = 0;
    }
    //Damages the ship according to the speed at which it collided at.
    m_health -= m_velocity.magnitude() * 0.15f;
    Vector2D collisionPosition = playerColShape->Intersection(*buildingColShape);
    Game::instance.m_objects.AddItem(new ParticleEmitter(
      collisionPosition, 0.1f, 3, MyDrawEngine::YELLOW, 100), false);
  }

  void Player::ProcessCollision(GameObject& other)
  {
    //Trackes when the last collision was to avoid damage bugs.
    static unsigned long lastCollisionFrame = 0; 
    if (other.GetType() == ObjectType::BUILDING && lastCollisionFrame != m_framesElapsed)
    {
      Segment2D* playerColShape = nullptr;
      Rectangle2D* buildingColShape = nullptr;
      FindCollisionObjects(other, playerColShape, buildingColShape);

      //Damages the ship only if it was moving too fast.
      if (m_velocity.magnitude() > COLLISION_THRESHOLD)
      {
        DamageShip(playerColShape, buildingColShape);
      }

      //Checks tilt to see whether the ship can right itself.
      if (m_angle < LEFT_TILT_ANGLE || m_angle > RIGHT_TILT_ANGLE)
      {
        m_targetAngle = 0.0f;
      }
      else if (m_angle > QUARTER_TURN_ANGLE * 2.0f)
      {
        m_targetAngle = QUARTER_TURN_ANGLE * 3.0f;
      }
      else
      {
        m_targetAngle = QUARTER_TURN_ANGLE;
      }

      Vector2D collisionNormal = buildingColShape->CollisionNormal(*playerColShape);
      collisionNormal.XValue = -collisionNormal.XValue;   //Must be swapped for reasons unknown.
      //Some mathsy thing.
      m_velocity = -(2.0f * (collisionNormal * m_velocity) * collisionNormal - m_velocity);

      MovePlayerOutCollision(playerColShape, buildingColShape, collisionNormal);
      m_timeSinceCollision = 0.0f;
      lastCollisionFrame = m_framesElapsed;
    }
    else if (other.GetType() == ObjectType::HELICOPTER || other.GetType() == ObjectType::PLANE)
    {
      Damage(MAX_HEALTH + 1.0f);
    }
  }

  void Player::MovePlayerOutCollision(const Segment2D* playerColShape, 
    const Rectangle2D* buildingColShape, Vector2D collisionNormal)
  {
    Segment2D segment;
    segment.PlaceAt(
      playerColShape->GetStart() - collisionNormal,
      playerColShape->GetEnd() - collisionNormal);
    //moves the player further and further along the collision normal until it is no longer
    //colliding.
    while (buildingColShape->Intersects(segment))
    {
      m_position -= collisionNormal * 0.5f;;
      segment.PlaceAt(
        segment.GetStart() - collisionNormal * 0.5f,
        segment.GetEnd() - collisionNormal * 0.5f);
    }
  }

  unsigned int Player::GetAmmo() const
  {
    return m_ammo;
  }

  void Player::Damage(float damage)
  {
    m_health -= damage;
  }

  float Player::GetFuel() const
  {
    return m_fuel;
  }

  void Player::PlaceCamera(bool& disableThrust) const
  {
    disableThrust = false;
    //Determines the proper vector for the camera position.
    Vector2D cameraPosition = m_position;
    if (cameraPosition.XValue < CAMERA_BACK_CONSTRAINT)
    {
      cameraPosition.XValue = CAMERA_BACK_CONSTRAINT;
    }
    if (cameraPosition.YValue < CAMERA_DOWN_CONSTRAINT)
    {
      cameraPosition.YValue = CAMERA_DOWN_CONSTRAINT;
    }
    else if (cameraPosition.YValue > CAMERA_UP_CONSTRAINT)
    {
      cameraPosition.YValue = CAMERA_UP_CONSTRAINT;
      disableThrust = true;
    }
    cameraPosition.YValue *= -1.0f;
    MyDrawEngine::GetInstance()->theCamera.PlaceAt(cameraPosition);
  }

  void Player::HandleRotation(MyInputs* input, float frameTime)
  {
    if (m_fuel > 0.0f)
    {
      //Handles player inputs
      if (((input->KeyPressed(DIK_D) && !m_controlsInverted) ||
        (input->KeyPressed(DIK_A) && m_controlsInverted)) &&
        m_timeSinceCollision > COLLIDED_TURN_WAIT)
      {
        m_angle += TURN_SPEED * frameTime;
        //Shows thruster on the left side.
        Vector2D left = m_position + Vector2D(-THRUST_SIDE, THRUST_BOTTOM).rotatedBy(-m_angle);
        Game::instance.m_objects.AddItem(new Explosion(
          left, THRUSTER_SCALE, THRUSTER_SPEED, THRUSTER_START_FRAME), false);
        m_fuel -= frameTime * THRUST_FUEL_COST;
        m_targetAngle = NO_TARGET;
      }
      else if (((input->KeyPressed(DIK_D) && m_controlsInverted) || 
        (input->KeyPressed(DIK_A) && !m_controlsInverted)) &&
        m_timeSinceCollision > COLLIDED_TURN_WAIT)
      {
        m_angle -= TURN_SPEED * frameTime;
        //Shows thruster on the right side.
        Vector2D right = m_position + Vector2D(THRUST_SIDE, THRUST_BOTTOM).rotatedBy(-m_angle);
        Game::instance.m_objects.AddItem(new Explosion(
          right, THRUSTER_SCALE, THRUSTER_SPEED, THRUSTER_START_FRAME), false);
        m_fuel -= frameTime * THRUST_FUEL_COST;
        m_targetAngle = NO_TARGET;
      }

      //Handles all rotation for reaching the target angle.
	    if (m_targetAngle > NO_TARGET)
      {
        if (m_angle < m_targetAngle)
        {
          if (abs(m_angle + m_targetAngle) < QUARTER_TURN_ANGLE * 2.0f)
          {
            m_angle += TURN_SPEED * frameTime;
          }
          else
          {
            m_angle -= TURN_SPEED * frameTime;
          }
          if (m_angle > m_targetAngle)
          {
            m_angle = m_targetAngle;
			      m_targetAngle = NO_TARGET;
          }
        }
        else
        {
          if (abs(m_angle - m_targetAngle) < QUARTER_TURN_ANGLE * 2.0f)
          {
            m_angle -= TURN_SPEED * frameTime;
          }
          else
          {
            m_angle += TURN_SPEED * frameTime;
          }
          if (m_angle < m_targetAngle)
          {
            m_angle = m_targetAngle;
			      m_targetAngle = NO_TARGET;
          }
        }

        if (m_angle == m_targetAngle)
        {
          m_targetAngle = NO_TARGET;
        }
      }
    }
    //Ensures that the angles is between 0 and 360 degrees.
    while (m_angle > QUARTER_TURN_ANGLE * 4.0f)
    {
      m_angle -= QUARTER_TURN_ANGLE * 4.0f;
    }
    while (m_angle < 0.0f)
    {
      m_angle += QUARTER_TURN_ANGLE * 4.0f;
    }
  }

  void Player::HandleTranslation(MyInputs* input, float frameTime, bool disableThrust)
  {
    //Allows movement only if the player wants it and the game allows it.
    if (!m_needsReset && input->KeyPressed(DIK_W) && m_fuel > 0.0f && !disableThrust)
    {
      //Simulates basic movement of the player.
      Vector2D acceleration;
      acceleration.setBearing(m_angle, ACCELERATION + m_extraAcceleration);
      m_velocity += acceleration * frameTime;
      //Applies less gravity when the player is accelerating.
      m_velocity -= Game::instance.GetGameManager().GetGravity() * frameTime * 0.5f;
      m_fuel -= frameTime;
      m_targetAngle = NO_TARGET;

      //Shows middle thruster.
      Vector2D middle = m_position + Vector2D(0.0f, THRUST_BOTTOM).rotatedBy(-m_angle);
      Game::instance.m_objects.AddItem(new Explosion(
        middle, THRUSTER_SCALE, THRUSTER_SPEED, THRUSTER_START_FRAME), false);

      //Plays the engine sound as and when necessary.
      if (!m_engineSoundPlaying)
      {
        MySoundEngine::GetInstance()->Play(s_engineSound, true);
        m_engineSoundPlaying = true;
      }
    }
    else
    {
      //Applies normal amount of gravity.
      m_velocity -= Game::instance.GetGameManager().GetGravity() * frameTime;
      //Stops playing the thruster sound.
      if (m_engineSoundPlaying)
      {
        MySoundEngine::GetInstance()->Stop(s_engineSound);
        m_engineSoundPlaying = false;
      }
    }
    if (m_position.XValue < PLAYER_BACK_CONSTRAINT)
    {
      m_velocity.XValue += (PLAYER_BACK_CONSTRAINT - m_position.XValue) * 2.0f;
    }
    m_velocity -= FRICTION * frameTime * m_velocity;
    m_position += m_velocity * frameTime;

    if (m_engineSoundPlaying)
    {
      //Increases thrust volume according to speed.
      MySoundEngine::GetInstance()->SetVolume(s_engineSound, -((int)THRUST_VOLUME_MINIMUM - 
        (int)(m_velocity.magnitude() * THRUST_VOLUME_MODIFIER)));
    }
  }

  void Player::HandleHealth()
  {
    if (m_health < 0.0f)
    {
      static const SoundIndex EXPLODE_SOUND = MySoundEngine::GetInstance()->LoadWav(
        L"Sounds/big_explosion.wav");
      MySoundEngine::GetInstance()->Play(EXPLODE_SOUND);
      Game::instance.GetGameManager().ResetGravity();

      if (m_healthLowSoundPlaying)
      {
        MySoundEngine::GetInstance()->Stop(s_healthLowSound);
      }
      //Creates a small a large explosion on death.
      Game::instance.m_objects.AddItem(new Explosion(
        m_position, EXPLOSION_SCALE, EXPLOSION_SPEED), false);
      Game::instance.m_objects.AddItem(new ParticleEmitter(
        m_position, EXPLOSION_PARTICLES_DURATION, EXPLOSION_PARTICLES_PER_SECOND, 
        MyDrawEngine::YELLOW, EXPLOSION_PARTICLES_FORCE), false);
      //Resets all necessary values.
      m_boostDuration = NO_TARGET;
      m_originalDistanceToTarget = m_position.XValue - 
        Game::instance.GetGameManager().GetPlayerSpawnPosition().XValue;
      m_position.YValue = PLAYER_RESET_YVALUE;
      m_velocity.set(0.0f, 0.0f);
      m_needsReset = true;
      m_timeUntilReset = PLAYER_RESET_WAIT;
      m_health = 0.0f;
      --m_lives;
    }
    //Plays a warning to the player when their health is low.
    else if (m_health > 0.0f && m_health <= LOW_HEALTH_THRESHOLD && !m_healthLowSoundPlaying)
    {
      MySoundEngine::GetInstance()->Play(s_healthLowSound, true);
      m_healthLowSoundPlaying = true;
    }
    //Stops the warning sound when the health increases past the threshold.
    else if (m_health > LOW_HEALTH_THRESHOLD && m_healthLowSoundPlaying)
    {
      MySoundEngine::GetInstance()->Stop(s_healthLowSound);
      m_healthLowSoundPlaying = false;
    }
  }

  void Player::Update(float frameTime)
  {
    //Change all time related variables
    ++m_framesElapsed;
    m_timeSinceCollision += frameTime;
    m_timeSinceLastShot += frameTime;
    m_boostDuration -= frameTime;
    m_lastFrameTime = frameTime;
    m_timeUntilReset -= frameTime;

    //Handle movements based on player input and previous collisions
    MyInputs* input = MyInputs::GetInstance();
    input->SampleKeyboard();
    bool disableThrust;
    PlaceCamera(disableThrust);
    HandleRotation(input, frameTime);
    HandleTranslation(input, frameTime, disableThrust);

    //Increase score
    if ((int)(m_position.XValue - GameManager::PLAYER_START.XValue) > m_highestScore)
    {
      m_highestScore = (int)(m_position.XValue - GameManager::PLAYER_START.XValue);
    }

    //Shooting
    if (!m_needsReset && input->KeyPressed(DIK_SPACE) && 
      m_timeSinceLastShot >= WAIT_TIME_PER_SHOT && m_ammo > 0)
    {
      m_timeSinceLastShot = 0.0f;
      Vector2D position = m_position + Vector2D(0.0f, -THRUST_BOTTOM).rotatedBy(-m_angle);
      Game::instance.m_objects.AddItem(new Projectile(
        position, m_angle, PROJECTILE_SHOOT_SPEED, true), true);
      --m_ammo;
    }

    //Self destructing
    if (input->NewKeyPressed(DIK_RETURN) && !m_needsReset)
    {
      m_health = -1.0f;
    }

    //Removing pickup boosts
    if (m_boostDuration <= 0.0f)
    {
      m_extraAcceleration = 0.0f;
      m_controlsInverted = false;
    }

    //Handles anything to do with health
    HandleHealth();
    //Handles resetting
    HandleReset(frameTime);
  }

  void Player::HandleReset(float frameTime)
  {
    if (m_needsReset && m_timeUntilReset <= 0.0f && m_timeUntilReset > -1.0f)
    {
      if (m_lives > 0)
      {
        static const SoundIndex RESPAWN_SOUND = MySoundEngine::GetInstance()->LoadWav(
          L"Sounds/player_respawn.wav");
        MySoundEngine::GetInstance()->Play(RESPAWN_SOUND);
        m_targetXPosition = Game::instance.GetGameManager().GetPlayerSpawnPosition().XValue;
      }
      //Triggers game ending.
      else
      {
        Game::instance.ShowScores();
      }
    }

    //Moves the player to their last refuel building.
    if (m_targetXPosition > 0.0f)
    {
      m_position.XValue -= m_originalDistanceToTarget * frameTime * 0.5f;
      //Waits until the player has observed their death for a period of time, and then waits
      //more until the player has finished moving back to their respawn point.
      if (m_timeUntilReset <= -PLAYER_RESET_WAIT)
      {
        m_position.XValue = m_targetXPosition;
        m_position.YValue = Game::instance.GetGameManager().GetPlayerSpawnPosition().YValue;
        m_targetXPosition = 0.0f;
        m_velocity.set(0.0f, 0.0f);
        m_angle = 0.0f;
        m_health = MAX_HEALTH;
        m_ammo = MAX_AMMO;
        m_fuel = MAX_FUEL;
        m_needsReset = false;
      }
    }
  }

  int Player::GetHighestDistanceScore() const
  {
    return m_highestScore;
  }

  void Player::InvertControls()
  {
    m_controlsInverted = true;
    m_boostDuration = BOOST_DURATION;
  }

  unsigned int Player::GetLives() const
  {
    return m_lives;
  }

  float Player::GetHealth() const
  {
    return m_health;
  }

  void Player::AddExplosionForce(Vector2D force)
  {
    //Moves and damages the player.
    m_velocity += force;
    m_health -= force.magnitude() * HEALTH_REDUCTION_MODIFIER;
  }

  Circle2D& Player::GetBoundingCircle()
  {
    m_boundingCircle.PlaceAt(m_position, 
      sqrtf((m_dimensions.XValue * m_dimensions.XValue) +
      (m_dimensions.YValue * m_dimensions.YValue)) / 2.0f);
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Player::GetCollisionShapes()
  {
    //Moves all the segments into the appropriate locations to represent the ships collision shape.
    Vector2D corners[COLLISION_SEGMENTS_COUNT];
    corners[0] = m_dimensions / -2.0f;
    corners[1].set(m_dimensions.XValue / 2.0f, m_dimensions.YValue / -2.0f);
    corners[2] = m_dimensions / 2.0f;
    corners[3].set(m_dimensions.XValue / -2.0f, m_dimensions.YValue / 2.0f);
    for (unsigned int i = 0; i < COLLISION_SEGMENTS_COUNT; ++i)
    {
      m_collisionSegments[i].PlaceAt(
        m_position + corners[i].rotatedBy(-m_angle), 
        m_position + corners[(i + 1) % COLLISION_SEGMENTS_COUNT].rotatedBy(-m_angle));
    }

    return m_collisionShapes;
  }
}