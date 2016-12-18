#include "EmptyBuilding.h"

namespace Voxtric
{
  const float EmptyBuilding::BUILDING_BOTTOM = -32.0f;

  EmptyBuilding::EmptyBuilding(Vector2D position)
  {
    m_dimensions.set((float)(rand() % MAX_WIDTH), 0.0f);
    Initialise(position);
  }

  EmptyBuilding::EmptyBuilding(Vector2D position, float width)
  {
    m_dimensions.set(width, 0.0f);
    Initialise(position);
  }

  void EmptyBuilding::Initialise(Vector2D position)
  {
    m_position = position;
    m_position.XValue += m_dimensions.XValue / 2.0f;
    PlaceBoundingCircle();
    //Initialises the building where the player can land on it.
    m_buildingMain.PlaceAt(
      Vector2D(m_position.XValue - (m_dimensions.XValue / 2.0f), BUILDING_BOTTOM), 
      Vector2D(m_position.XValue + (m_dimensions.XValue / 2.0f), 0.0f));
    m_collisionShapes.emplace_back(&m_buildingMain);
    m_index = EMPTY_BUILDING_INDEX;
  }
}