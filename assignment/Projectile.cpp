#include "Projectile.h"

#include "Player.h"
#include "Explosion.h"
#include "gamecode.h"
#include "ParticleEmitter.h"

namespace Voxtric
{
  const float Projectile::PROJECTILE_RADIUS = 42.0f;
  const float Projectile::IMAGE_SCALE = 0.8f;
  const float Projectile::EXPLOSION_SCALE = 0.75f;
  const float Projectile::EXPLOSION_SPEED = 3.0f;
  const float Projectile::EXPLOSION_DURATION = 0.3f;
  const float Projectile::DAMAGE_DEALT = 15.0f;

  SoundIndex Projectile::s_shootSounds[SIMULTANEOUS_SOUNDS];
  unsigned int Projectile::s_shootSoundIndex = 0;
  SoundIndex Projectile::s_explodeSounds[SIMULTANEOUS_SOUNDS];
  unsigned int Projectile::s_explodeSoundIndex = 0;

  Projectile::Projectile(Vector2D position, float angle, float speed, bool playerFired, 
    float waitForBuildingCollide) : GameObject(ObjectType::PROJECTILE)
  {
    //Loads the sounds only once.
    static bool l_loadSounds = true;
    if (l_loadSounds)
    {
      for (unsigned int i = 0; i < SIMULTANEOUS_SOUNDS; ++i)
      {
        s_shootSounds[i] = MySoundEngine::GetInstance()->LoadWav(L"Sounds/projectile_shoot.wav");
        s_explodeSounds[i] = MySoundEngine::GetInstance()->LoadWav(L"Sounds/small_explosion.wav");
      }
      l_loadSounds = false;
    }
    MySoundEngine::GetInstance()->Play(s_shootSounds[s_shootSoundIndex]);
    //Increases the index of the sound to be played.
    s_shootSoundIndex = s_shootSoundIndex == SIMULTANEOUS_SOUNDS - 1 ? 0 : s_shootSoundIndex + 1;

    m_waitForBuildingCollide = waitForBuildingCollide;
    m_playerFired = playerFired;
    m_position = position;
    m_velocity = Vector2D(0.0f, speed).rotatedBy(-angle);
    
    //Loads all images for animation.
    for (unsigned int i = 0; i < FRAME_COUNT; ++i)
    {
      std::wstring fullPath = L"Textures/energy_ball/energy_ball_" + std::to_wstring(i) + L".bmp";
      LoadImage((wchar_t*)fullPath.c_str());
    }
    m_animationDirection = 1;
    m_angle = angle;
    m_collisionShapes.emplace_back(&m_boundingCircle);
    m_imageScale = IMAGE_SCALE;
    m_drawDepth = DRAW_DEPTH;
  }

  void Projectile::Update(float frameTime)
  {
    //Simulates basic projectile movement.
    m_velocity -= Game::instance.GetGameManager().GetGravity() * frameTime;
    m_position += m_velocity * frameTime;
    m_imageNumber += m_animationDirection;
    //Reverses direction of animation when end or beginning of animation is reached.
    if (m_imageNumber == 0 || m_imageNumber == FRAME_COUNT - 1)
    {
      m_animationDirection *= -1;
    }
    if (m_waitForBuildingCollide > 0.0f)
    {
      m_waitForBuildingCollide -= frameTime;
    }
  }

  void Projectile::ProcessCollision(GameObject& other)
  {
    //Different effects for collision with a player and collision with a building.
    if (other.GetType() == ObjectType::BUILDING && m_waitForBuildingCollide <= 0.0f)
    {
      Deactivate();
      //Creates an explosion effect.
      Game::instance.m_objects.AddItem(
        new Explosion(m_position, EXPLOSION_SCALE, EXPLOSION_SPEED), true);
      Game::instance.m_objects.AddItem(new ParticleEmitter(
        m_position, EXPLOSION_DURATION, PARTICLES_PER_SECOND, 
        MyDrawEngine::YELLOW, PARTICLES_EXPULSION_FORCE), false);
      //Allows multiple explosion sounds to be played at once.
      MySoundEngine::GetInstance()->Play(s_explodeSounds[s_explodeSoundIndex]);
      ++s_explodeSoundIndex;
      if (s_explodeSoundIndex == SIMULTANEOUS_SOUNDS)
      {
        s_explodeSoundIndex = 0;
      }
    }
    else if (other.GetType() == ObjectType::PLAYER && !m_playerFired)
    {
      Deactivate();
      ((Player*)&other)->Damage(DAMAGE_DEALT);
      //Creates a basic explosion effect.
      Game::instance.m_objects.AddItem(new Explosion(
        m_position, EXPLOSION_SCALE, EXPLOSION_SPEED), true);
      //Allows multiple explosion sounds to be played at once.
      MySoundEngine::GetInstance()->Play(s_explodeSounds[s_explodeSoundIndex]);
      ++s_explodeSoundIndex;
      if (s_explodeSoundIndex == SIMULTANEOUS_SOUNDS)
      {
        s_explodeSoundIndex = 0;
      }
    }
  }

  Circle2D& Projectile::GetBoundingCircle()
  {
    m_boundingCircle.PlaceAt(m_position, PROJECTILE_RADIUS * m_imageScale);
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Projectile::GetCollisionShapes()
  {
    return m_collisionShapes;
  }

  bool Projectile::FiredByPlayer() const
  {
    return m_playerFired;
  }
}