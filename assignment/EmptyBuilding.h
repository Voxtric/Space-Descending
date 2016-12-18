#pragma once

#include "Building.h"

namespace Voxtric
{
  class EmptyBuilding : public Building
  {
  public:
    static const BuildingIndex EMPTY_BUILDING_INDEX = 
      Building::DIFFERENT_RANDOM_BUILDING_COUNT + 1;  //Index to represent an empty building space.

    EmptyBuilding(Vector2D position);
    EmptyBuilding(Vector2D position, float width);

  private:
    static const unsigned int MAX_WIDTH = 300;  //Maximum width for a building space.
    static const float BUILDING_BOTTOM;         //Bottom of the collision box.

    void Initialise(Vector2D position);
  };
}