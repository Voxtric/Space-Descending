#pragma once

#include "objecttypes.h"
#include "vector2D.h"
#include "PickupType.hpp"

namespace Voxtric
{
  struct EntityInfo
  {
  public:
    const Vector2D position;
    const ObjectType entityType;
    const PickupType pickupType;
    bool spawned;
    bool removed;

    EntityInfo(Vector2D position, ObjectType entityType) : 
      position(position), entityType(entityType), pickupType(PickupType::None), spawned(true), 
      removed(false) {}

    EntityInfo(Vector2D position, ObjectType entityType, PickupType pickupType) : 
      position(position), entityType(entityType), pickupType(pickupType), spawned(true), 
      removed(false) {}

    EntityInfo operator=(const EntityInfo& enemyInfo)
    {
      EntityInfo newInfo(enemyInfo.position, enemyInfo.entityType, enemyInfo.pickupType);
      newInfo.spawned = enemyInfo.spawned;
      newInfo.removed = enemyInfo.removed;
      return newInfo;
    }
  };
}