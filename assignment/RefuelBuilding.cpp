#include "RefuelBuilding.h"

#include "Player.h"
#include "gamecode.h"

namespace Voxtric
{
  const float RefuelBuilding::WAIT_PER_REFUEL = 0.1f;

  SoundIndex RefuelBuilding::s_refuelSound = 0;

  RefuelBuilding::RefuelBuilding(Vector2D position)
  {
    //Loads the sound if it isn't already loaded.
    if (s_refuelSound == 0)
    {
      s_refuelSound = MySoundEngine::GetInstance()->LoadWav(L"Sounds/player_refuel.wav");
    }

    m_position = position;
    m_refuelSoundPlaying = false;
    m_index = REFUEL_BUILDING_INDEX;
    m_timeUntilRefuel = 0.0f;
    LoadImage(L"Textures/building_refuel.bmp");

    m_dimensions.set(166.0f, 410.0f);
    m_position.XValue += m_dimensions.XValue / 2.0f;

    //Places all collision related shapes.
    PlaceBoundingCircle();
    m_buildingMain.PlaceAt(
      Vector2D(m_position.XValue - (m_dimensions.XValue / 2.0f), m_position.YValue), 
      Vector2D(m_position.XValue + (m_dimensions.XValue / 2.0f), m_position.YValue + 354.0f));
    m_buildingTop.PlaceAt(
      Vector2D(m_position.XValue - (m_dimensions.XValue / 2.0f), m_position.YValue + 340.0f),
      Vector2D(m_position.XValue - 60.0f, m_position.YValue + m_dimensions.YValue));
    m_buildingOtherTop.PlaceAt(
      Vector2D(m_position.XValue + 60.0f, m_position.YValue + 340.0f), 
      Vector2D(m_position.XValue + (m_dimensions.XValue / 2.0f), 
               m_position.YValue + m_dimensions.YValue));
    m_collisionShapes.emplace_back(&m_buildingMain);
    m_collisionShapes.emplace_back(&m_buildingTop);
    m_collisionShapes.emplace_back(&m_buildingOtherTop);
  }

  void RefuelBuilding::OnPause()
  {
    //Stops the sound during the pause menu.
    if (m_refuelSoundPlaying)
    {
      m_refuelSoundPlaying = false;
      MySoundEngine::GetInstance()->Stop(s_refuelSound);
    }
  }

  void RefuelBuilding::Update(float frameTime)
  {
    //Stops the refuel sound once refueling is completed.
    if (m_refuelSoundPlaying && m_timeUntilRefuel < -1.1f * frameTime)
    {
      MySoundEngine::GetInstance()->Stop(s_refuelSound);
      m_refuelSoundPlaying = false;
    }

    //Despawns the building if it goes off the screen far enough.
    m_timeUntilRefuel -= frameTime;
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

  void RefuelBuilding::ProcessCollision(GameObject& other)
  {
    //Refuels the player in small intervals if the player has landed correctly.
    if (other.GetType() == ObjectType::PLAYER && m_timeUntilRefuel < 0.0f &&    
      ((Player*)&other)->CanRefuel())
    {
      //Places the refuel trigger only when it's needed.
      Rectangle2D refuelTrigger;
      refuelTrigger.PlaceAt(
        Vector2D(m_position.XValue - 60.0f, m_position.YValue + 354.0f),
        Vector2D(m_position.XValue + 60.0f, m_position.YValue + 360.0f));
      //Finds if any of the ships collision segments are actually colliding
      //with the trigger.
      bool triggered = false;
      std::vector<IShape2D*> collisionShapes = other.GetCollisionShapes();
      for (unsigned int i = 0; i < collisionShapes.size() && !triggered; ++i)
      {
        triggered = collisionShapes[i]->Intersects(refuelTrigger);
      }
      if (triggered)
      {
        //Plays the sound only if refueling and not already playing the sound.
        if (!m_refuelSoundPlaying)
        {
          m_refuelSoundPlaying = true;
          MySoundEngine::GetInstance()->Play(s_refuelSound, true);
        }
        ((Player*)&other)->Refuel();
        m_timeUntilRefuel = WAIT_PER_REFUEL;
      }
    }
  }
}