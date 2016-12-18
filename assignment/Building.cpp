#include "Building.h"

#include "gamecode.h"

namespace Voxtric
{
  Building::Building() : GameObject(ObjectType::BUILDING) {}

  //Creates a random building.
  Building::Building(Vector2D position) : GameObject(ObjectType::BUILDING)
  {
    m_index = rand() % DIFFERENT_RANDOM_BUILDING_COUNT;
    Initialise(position);
  }

  //Creates a specific building.
  Building::Building(Vector2D position, uint8_t index) : GameObject(ObjectType::BUILDING)
  {
    m_index = index;
    Initialise(position);
  }

  void Building::Initialise(Vector2D position)
  {
    m_position = Vector2D(position);
    ChooseBuilding();
    m_collisionShapes.emplace_back(&m_buildingMain);
    m_collisionShapes.emplace_back(&m_buildingTop);
  }

  Vector2D Building::GetDimensions() const
  {
    return m_dimensions;
  }

  void Building::ChooseBuilding()
  {
    //Sets appropriate building data from choice.
    switch (m_index)
    {
    case 0:
      m_dimensions.set(166.0f, 462.0f);
      PlaceCollisionShapes(409.0f, 44.0f);
      break;
    case 1:
      m_dimensions.set(166.0f, 340.0f);
      PlaceCollisionShapes(286.0f, 44.0f);
      break;
    case 2:
      m_dimensions.set(212.0f, 380.0f);
      PlaceCollisionShapes(358.0f, 48.0f);
      break;
    case 3:
      m_dimensions.set(216.0f, 332.0f);
      PlaceCollisionShapes(301.0f, 44.0f);
      break;
    case 4:
      //Building has different layout to others, so has manual setup.
      m_dimensions.set(270.0f, 184.0f);
      m_position.XValue += m_dimensions.XValue / 2.0f;
      PlaceBoundingCircle();
      m_buildingMain.PlaceAt(
        Vector2D(m_position.XValue - (m_dimensions.XValue / 2.0f), m_position.YValue),
        Vector2D(m_position.XValue + (m_dimensions.XValue / 2.0f),
          m_position.YValue + m_dimensions.YValue));
      m_buildingTop.PlaceAt(
        Vector2D(m_position.XValue - (m_dimensions.XValue / 2.0f), m_position.YValue),
        Vector2D(m_position.XValue + (m_dimensions.XValue / 2.0f), 
          m_position.YValue + m_dimensions.YValue));
      break;
    }
    std::wstring texture = L"Textures/building_" + std::to_wstring(m_index) + L".bmp";
    LoadImage((wchar_t*)texture.c_str());
  }

  BuildingInfo Building::GetInfo() const
  {
    return BuildingInfo(m_index, m_position.XValue, m_dimensions);
  }

  void Building::PlaceBoundingCircle()
  {
    m_boundingCircle.PlaceAt(
      Vector2D(m_position.XValue, m_position.YValue + (m_dimensions.YValue / 2.0f)),
        0.5f * sqrtf((m_dimensions.XValue * m_dimensions.XValue) + 
        (m_dimensions.YValue * m_dimensions.YValue)));
  }

  void Building::PlaceCollisionShapes(float mainHeight, float topHalfWidth)
  {
    //Places the collision shapes based on the height and width of the different collision
    //rectangles.
    m_position.XValue += m_dimensions.XValue / 2.0f;
    PlaceBoundingCircle();
    m_buildingMain.PlaceAt(
      Vector2D(m_position.XValue - (m_dimensions.XValue / 2.0f), m_position.YValue), 
      Vector2D(m_position.XValue + (m_dimensions.XValue / 2.0f), m_position.YValue + mainHeight));
    m_buildingTop.PlaceAt(
      Vector2D(m_position.XValue - topHalfWidth, m_position.YValue + mainHeight), 
      Vector2D(m_position.XValue + topHalfWidth, m_position.YValue + m_dimensions.YValue));
  }

  void Building::ProcessCollision(GameObject& other) {} //Unused.

  void Building::Update(float frameTime)
  {
    //Despawns the building once it has gone far enough off the screen.
    float originalX = m_position.XValue - (m_dimensions.XValue / 2.0f);
    if (originalX + GameManager::DESPAWN_BUFFER_DISTANCE < 
      MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue)
    {
      Game::instance.GetGameManager().IncreaseBackBuildingIndex();
      Deactivate();
    }
    else if (originalX - GameManager::DESPAWN_BUFFER_DISTANCE >
      MyDrawEngine::GetInstance()->GetViewport().GetBottomRight().XValue)
    {
      Game::instance.GetGameManager().ReduceForwardBuildingIndex();
      Deactivate();
    }
  }

  Circle2D& Building::GetBoundingCircle()
  {
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& Building::GetCollisionShapes()
  {
    return m_collisionShapes;
  }
}