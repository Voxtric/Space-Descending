#pragma once

#include "Building.h"
#include "mysoundengine.h"

namespace Voxtric
{
  class RefuelBuilding : public Building
  {
  public:
    //The index used for a refuel building.
    static const BuildingIndex REFUEL_BUILDING_INDEX = Building::DIFFERENT_RANDOM_BUILDING_COUNT;

    RefuelBuilding(Vector2D position);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    void OnPause() override;

  private:
    static const float WAIT_PER_REFUEL; //Time between each fuel and ammo increase.

    static SoundIndex s_refuelSound;

    Rectangle2D m_buildingOtherTop;
    float m_timeUntilRefuel;
    bool m_refuelSoundPlaying;
  };
}