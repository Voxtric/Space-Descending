#pragma once

#include "GameObject.h"
#include "BuildingInfo.hpp"

namespace Voxtric
{
  class Building : public GameObject
  {
  public:
    //Number of different building images for normal buildings.
    static const unsigned int DIFFERENT_RANDOM_BUILDING_COUNT = 5; 

    Building();
    Building(Vector2D position);
    Building(Vector2D position, BuildingIndex index);

    void Update(float frameTime) override;
    virtual void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

    Vector2D GetDimensions() const;
    BuildingInfo GetInfo() const;

  protected:
    Rectangle2D m_buildingMain;
    Rectangle2D m_buildingTop;
    Vector2D m_dimensions;
    BuildingIndex m_index;

    void PlaceBoundingCircle();
    void Initialise(Vector2D position);

  private:
    void ChooseBuilding();
    void PlaceCollisionShapes(float mainHeight, float topHalfWidth);
  };
}