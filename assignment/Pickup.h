#pragma once

#include "GameObject.h"
#include "gamecode.h"
#include "Player.h"
#include "EntityInfo.hpp"

#define ACTIVE_PLAYER Game::instance.GetGameManager().GetPlayer() //Gets the active player.

namespace Voxtric
{
  class Pickup : public GameObject
  {
  public:
    static const unsigned int TYPE_COUNT = 5; //Number of different pickup types.
    static const float TEXTURE_WIDTH; //Width of all pickup textures.

    Pickup(Vector2D position, PickupType pickupType);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

    EntityInfo GetInfo() const;

  private:
    const PickupType PICKUP_TYPE;

    Rectangle2D m_collisionShape;

    virtual void Activate() = 0;  //Triggers pickup effect.
  };
}