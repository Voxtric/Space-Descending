#include "Pickup.h"

#include "mysoundengine.h"

namespace Voxtric
{
  const float Pickup::TEXTURE_WIDTH = 32.0f;

  Pickup::Pickup(Vector2D position, PickupType pickupType) :
    PICKUP_TYPE(pickupType), GameObject(ObjectType::PICKUP)
  {
    m_position = position;
    m_collisionShapes.push_back(&m_collisionShape);
  }

  void Pickup::Update(float frameTime)
  {
    //Despawns the pickup if it goes off the screen.
    if (m_position.XValue + GameManager::DESPAWN_BUFFER_DISTANCE <
      MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue || 
      m_position.XValue - GameManager::DESPAWN_BUFFER_DISTANCE >
      MyDrawEngine::GetInstance()->GetViewport().GetBottomRight().XValue)
    {
      Deactivate();
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, false);
    }
  }

  void Pickup::ProcessCollision(GameObject& other)
  {
    if (other.GetType() == ObjectType::PLAYER)
    {
      Deactivate();
      Activate(); //Effect of the pickup is activated.
      Game::instance.GetGameManager().RemoveEntity(m_position.XValue, true);
      static const SoundIndex ACTIVATE_SOUND = MySoundEngine::GetInstance()->LoadWav(
        L"Sounds/pickup_activate.wav");
      MySoundEngine::GetInstance()->Play(ACTIVATE_SOUND);
    }
  }

  Circle2D& Pickup::GetBoundingCircle()
  {
    m_boundingCircle.PlaceAt(m_position, TEXTURE_WIDTH * 2.0f);
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Pickup::GetCollisionShapes()
  {
    m_collisionShape.PlaceAt(
      m_position - Vector2D(TEXTURE_WIDTH, TEXTURE_WIDTH) * m_imageScale,
      m_position + Vector2D(TEXTURE_WIDTH, TEXTURE_WIDTH) * m_imageScale);
    return m_collisionShapes;
  }

  EntityInfo Pickup::GetInfo() const
  {
    return EntityInfo(m_position, TYPE, PICKUP_TYPE);
  }
}