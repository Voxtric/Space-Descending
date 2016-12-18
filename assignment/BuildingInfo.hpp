#pragma once

#include <cstdint>

namespace Voxtric
{
  typedef uint8_t BuildingIndex;

  struct BuildingInfo
  {
    const BuildingIndex index;
    const float xPos;
    const Vector2D dimensions;

    BuildingInfo(BuildingIndex index, float xPos, Vector2D dimensions) : 
      index(index), xPos(xPos), dimensions(dimensions) {}
  };
}