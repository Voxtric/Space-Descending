#include "Explosion.h"

#include "gamecode.h"

namespace Voxtric
{
  const float Explosion::EXPLOSION_FORCE = 50.0f;

  Explosion::Explosion(Vector2D position, float scale, float loopsPerSecond,
    unsigned int startFrame) : GameObject(ObjectType::EXPLOSION)
  {
    m_frame = (float)startFrame / (float)FRAME_COUNT;
    m_position = position;
    m_imageScale = scale;
    m_loopsPerSecond = loopsPerSecond;
    //Loads all of the images for the animation.
    std::wstring path = L"Textures/explosion/explosion_";
    for (unsigned int i = 0; i < FRAME_COUNT; ++i)
    {
      std::wstring fullPath = path + std::to_wstring(i) + L".bmp";
      LoadImage((wchar_t*)fullPath.c_str());
    }
    m_collisionShapes.emplace_back(&m_boundingCircle);

    //Ensures that explosions appear over everything apart from when they represent thrusters.
    if (startFrame == 0)
    {
      m_drawDepth = 100;
    }
  }

  void Explosion::ProcessCollision(GameObject& other)
  {
    //Forces the played back if they're too close.
    if (other.GetType() == ObjectType::PLAYER)
    {
      Player* player = (Player*)&other;
      Vector2D vecDiff = player->GetPosition() - m_position;
      player->AddExplosionForce(vecDiff * EXPLOSION_FORCE / vecDiff.magnitude());
    }
  }

  void Explosion::Update(float frameTime)
  {
    //Moves the animation along at the speed requested.
    m_frame += frameTime * m_loopsPerSecond;
    //Deactivates the explosion once the animation is over.
    if (m_frame * FRAME_COUNT >= FRAME_COUNT)
    {
      Deactivate();
    }
    m_imageNumber = (int)(m_frame * FRAME_COUNT);
  }

  Circle2D& Explosion::GetBoundingCircle()
  {
    m_boundingCircle.PlaceAt(m_position, EXPLOSION_FORCE * m_imageScale);
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Explosion::GetCollisionShapes()
  {
    return m_collisionShapes;
  }
}