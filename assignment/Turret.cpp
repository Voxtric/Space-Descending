#include "Turret.h"

#include "gamecode.h"
#include "Projectile.h"
#include "Explosion.h"

namespace Voxtric
{
  const float Turret::DELAY_PER_SHOT = 1.5f;
  const float Turret::PROJECTILE_SHOOT_SPEED = 1250.0f;
  const float Turret::QUARTER_TURN_ANGLE = 1.5708f;
  const float Turret::RADIUS = 22.0f;

  Turret::Turret(Vector2D position) : Enemy(position, SCORE_MODIFIER, ObjectType::TURRET)
  {
    LoadImage(L"Textures/turret.bmp");
    m_drawDepth = 10; //In front of most things.
	//Random time period that is less than a second.
    m_timeSinceLastShot = (rand() % (int)(DELAY_PER_SHOT * PLAYER_ATTACK_WAIT_MILLISECONDS)) / 
      (float)PLAYER_ATTACK_WAIT_MILLISECONDS; 
    m_boundingCircle.PlaceAt(m_position, RADIUS);  //Turret radius
    m_collisionShapes.emplace_back(&m_boundingCircle);
  }

  void Turret::ProcessCollision(GameObject& other)
  {
    if (other.GetType() == ObjectType::PROJECTILE)
    {
      //Destroys itself only if the shot was fired by the player.
      if (((Projectile*)&other)->FiredByPlayer())
      {
        other.Deactivate();
        Explode(EXPLOSION_SCALE);
        Game::instance.GetGameManager().GetPlayer().AddScore(m_scoreAddition);
		//Removes the turret from the spawnable enemies.
        Game::instance.GetGameManager().RemoveEntity(m_position.XValue, true);

		//Allows the same sound to overlap.
        MySoundEngine::GetInstance()->Play(s_explodeSounds[s_explodeSoundIndex]);
        ++s_explodeSoundIndex;
        if (s_explodeSoundIndex == SIMULTANEOUS_EXPLOSIONS)
        {
          s_explodeSoundIndex = 0;
        }
      }
    }
    else if (other.GetType() == ObjectType::GRINDER)
    {
      Explode(3.0f);
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, true);
    }
  }

  void Turret::Update(float frameTime)
  {
    Enemy::Update(frameTime);

    Player player = Game::instance.GetGameManager().GetPlayer();
    //Checks that the player is actually alive so can be aimed at.
    if (player.GetPosition().YValue > 0.0f)
    {
      //Determines the angle facing the player.
      Vector2D diffVec = Game::instance.GetGameManager().GetPlayer().GetPosition() - m_position;
      float dotProduct = (m_position.XValue * diffVec.XValue) + 
        (m_position.YValue * diffVec.YValue);
      float determinant = (m_position.XValue * diffVec.YValue) - 
        (m_position.YValue * diffVec.XValue);
      m_angle = -atan2(determinant, dotProduct) + QUARTER_TURN_ANGLE;

      m_timeSinceLastShot += frameTime;
      if (m_timeSinceLastShot >= DELAY_PER_SHOT)
      {
        //Wait of 0.1 seconds ensures the shot doesn't get instantly destroyed.
        Projectile* projectile = new Projectile(m_position, m_angle, PROJECTILE_SHOOT_SPEED, 
          false, 0.1f);  
        Game::instance.m_objects.AddItem(projectile, true);
        m_timeSinceLastShot = 0.0f;
      }
    }
  }
  
  void Turret::TryDespawn()
  {
    //Despawns the turret if it is off the screen.
    if (m_position.XValue + GameManager::DESPAWN_BUFFER_DISTANCE < 
      MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue || 
      m_position.XValue - GameManager::DESPAWN_BUFFER_DISTANCE > 
      MyDrawEngine::GetInstance()->GetViewport().GetBottomRight().XValue)
    {
      Deactivate();
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, false);
    }
  }

  Circle2D& Turret::GetBoundingCircle()
  {
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Turret::GetCollisionShapes()
  {
    return m_collisionShapes;
  }
}