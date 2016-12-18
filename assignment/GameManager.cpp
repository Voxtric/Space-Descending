#include "GameManager.h"

#include <random>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

#include "gamecode.h"
#include "RefuelBuilding.h"
#include "EmptyBuilding.h"
#include "Explosion.h"
#include "Helicopter.h"
#include "Turret.h"
#include "Plane.h"
#include "ParticleEmitter.h"
#include "myinputs.h"
#include "Pickups.hpp"
#include "mysoundengine.h"
#include "Grinder.h"

namespace Voxtric
{
  const Vector2D GameManager::PLAYER_START = Vector2D(2500.0f, -200.0f);
  const float GameManager::DIFFICULTY_INCREASE_DISTANCE = 6000.0f;
  const float GameManager::MAX_DISTANCE_PER_REFUEL_BUILDING = 6000.0f;
  const float GameManager::DESPAWN_BUFFER_DISTANCE = 300.0f;
  const float GameManager::TOP_BUILDING_HEIGHT = 800.0f;
  const float GameManager::PARALAX_MULTIPLIER = -0.3f;

  GameManager::GameManager() : GameObject(ObjectType::UNKNOWN)
  {
    //Ensures pointer is never pointing to anything it shouldn't be.
    m_player = nullptr;
    m_lastDifficultySpawnedAt = 0;
    srand((unsigned int)time(NULL));
    m_drawDepth = -100;
    m_worldGravity.set(0.0f, 1000.0f);
    m_gravityTimer = 0.0f;
    m_distanceSinceLastRefuelBuilding = 0.0f;
  }

  void GameManager::RandomiseGravity()
  {
    m_worldGravity.setBearing((rand() % 360) * 3.14 / 180.0f, 1000.0f);
    m_gravityTimer = 0.0f;
  }

  Vector2D GameManager::GetGravity() const
  {
    return m_worldGravity;
  }

  void GameManager::IncreaseParalaxOffset(Vector2D increase)
  {
    m_paralaxOffset += increase;
  }

  const int* GameManager::GetHighScores() const
  {
    return &m_highScores[0];
  }

  //Initialises all game components.
  void GameManager::StartGame()
  {
    m_player = new Player(PLAYER_START);
    Game::instance.m_objects.AddItem(m_player, true);

    m_nextForwardBuildingIndex = 0;
    m_nextBackBuildingIndex = -1;
    m_difficulty = 0;
    m_needsRefuelBuilding = true;

    //Loads in previously saved high scores.
    for (unsigned int i = 0; i < SCORES_TO_SAVE; ++i)
    {
      m_highScores[i] = 0;
    }
    std::ifstream scores("scores.txt");
    unsigned int i = 0;
    std::string score;
    while (i < SCORES_TO_SAVE && std::getline(scores, score))
    {
      //Tests whether the value is an actual number.
      bool isNumber = true;
      //Loops through all the characters to check if they are a digit.
      for (unsigned int j = 0; j < score.size() && isNumber; ++j)
      {
        if (!isdigit(score.c_str()[j]))
        {
          isNumber = false;
        }
      }
      if (isNumber)
      {
        m_highScores[i] = atoi(score.c_str());
      }
      else
      {
        break;
      }
      ++i;
    }
    scores.close();
  }

  Vector2D GameManager::GetParalaxOffset() const
  {
    return m_paralaxOffset;
  }

  void GameManager::UpdateHighScores()
  {
    //Re-orders the high scores to take into account the most recent score.
    bool highScoreAdded = false;
    for (unsigned int i = 0; i < SCORES_TO_SAVE && !highScoreAdded; ++i)
    {
      if (m_player->GetScore() > m_highScores[i])
      {
        highScoreAdded = true;
        for (unsigned int j = SCORES_TO_SAVE - 1; j > i; --j)
        {
          m_highScores[j] = m_highScores[j - 1];
        }
        m_highScores[i] = m_player->GetScore();
      }
    }
    //Re-writes the high score file if the new score was added to the list.
    if (highScoreAdded)
    {
      std::ofstream scores("scores.txt");
      for (unsigned int i = 0; i < SCORES_TO_SAVE; ++i)
      {
        scores << m_highScores[i] << std::endl;
      }
      scores.close();
    }
  }

  void GameManager::ReduceForwardBuildingIndex()
  {
    --m_nextForwardBuildingIndex;
  }

  void GameManager::IncreaseBackBuildingIndex()
  {
    ++m_nextBackBuildingIndex;
  }

  void GameManager::Draw()
  {
    Game::instance.GetGUI().DrawTiledBackground(true);
    Game::instance.GetGUI().DrawHUD();        
  }

  void GameManager::SpawnEnemy(const Building* building, float nextForwardPosition)
  {
    Enemy* enemy = nullptr;
    //Chooses the enemy type to spawn.
    unsigned int enemyType = rand() % Enemy::TYPE_COUNT;
    switch (enemyType)
    {
      //Spawns a helicopter.
    case 0:
      enemy = new Helicopter(Vector2D(
        nextForwardPosition,
        MyDrawEngine::GetInstance()->GetViewport().GetTopLeft().YValue)), true;
      m_entities.push_back(enemy->GetInfo());
      break;
      //Spawns a turret.
    case 1:
      enemy = new Turret(Vector2D(
        nextForwardPosition + (building->GetDimensions().XValue * 0.5f),
        building->GetDimensions().YValue)), true;
      m_entities.push_back(enemy->GetInfo());
      break;
      //Spawns a plane.
    case 2:
      enemy = new Plane(Vector2D(
        nextForwardPosition,
        (float)(rand() % PLANE_SPAWN_HEIGHT_RANGE) + TOP_BUILDING_HEIGHT));
      break;
    }
    Game::instance.m_objects.AddItem(enemy, true);
  }

  void GameManager::SpawnPickup(const Building* building, float nextForwardPosition)
  {
    //Chooses what pickup type to spawn.
    Pickup* pickup = nullptr;
    switch (rand() % Pickup::TYPE_COUNT)
    {
      //Speed boost pickup.
    case 0:
      pickup = new SpeedBoostPickup(Vector2D(
        nextForwardPosition + (building->GetDimensions().XValue * 0.5f),
        building->GetDimensions().YValue + Pickup::TEXTURE_WIDTH));
      break;
      //Health boost pickup.
    case 1:
      pickup = new HealthBoostPickup(Vector2D(
        nextForwardPosition + (building->GetDimensions().XValue * 0.5f),
        building->GetDimensions().YValue + Pickup::TEXTURE_WIDTH));
      break;
      //Life boost pickup.
    case 2:
      pickup = new LifeBoostPickup(Vector2D(
        nextForwardPosition + (building->GetDimensions().XValue * 0.5f),
        building->GetDimensions().YValue + Pickup::TEXTURE_WIDTH));
      break;
      //Score boost pickup.
    case 3:
      pickup = new ScoreBoostPickup(Vector2D(
        nextForwardPosition + (building->GetDimensions().XValue * 0.5f),
        building->GetDimensions().YValue + Pickup::TEXTURE_WIDTH));
      break;
      //Random boost pickup.
    case 4:
      pickup = new RandomBoostPickup(Vector2D(
        nextForwardPosition + (building->GetDimensions().XValue * 0.5f),
        building->GetDimensions().YValue + Pickup::TEXTURE_WIDTH));
      break;
    }
    Game::instance.m_objects.AddItem(pickup, true);
    m_entities.push_back(pickup->GetInfo());
  }

  void GameManager::GenerateBuildingsAhead()
  {
    //Creates a starting building at the beginning of each game.
    if (m_buildings.size() == 0)
    {
      Building* building = new Building(Vector2D(0.0f, 0.0f));
      m_buildings.push_back(building->GetInfo());
      ++m_nextForwardBuildingIndex;
      Game::instance.m_objects.AddItem(building, true);
    }

    //Checks on information for different buildings to load.
    BuildingInfo info = m_buildings[m_nextForwardBuildingIndex - 1];
    float nextForwardPosition = info.xPos + (info.dimensions.XValue * 0.5f);
    //Fills screen with as many buildings as is necessary.
    while (nextForwardPosition - DESPAWN_BUFFER_DISTANCE <
      MyDrawEngine::GetInstance()->GetViewport().GetBottomRight().XValue)
    {
      Building* building = nullptr;
      //Checks whether the building has been decided once before or needs to be newly created.
      if (m_nextForwardBuildingIndex >= (int)m_buildings.size())
      {
        //Chooses what building to create.
        unsigned int specialChance = 0;
        //Chooses a refuel building if one has to be made.
        if ((m_needsRefuelBuilding && nextForwardPosition + DESPAWN_BUFFER_DISTANCE > 
          m_player->GetPosition().XValue) ||
          m_distanceSinceLastRefuelBuilding >= MAX_DISTANCE_PER_REFUEL_BUILDING)
        {
          specialChance = FUEL_BUILDING_CHANCE;
        }
        else
        {
          //Forces normal buildings to be created in the space before the player spawns.
          do
          {
            specialChance = rand() % SPECIAL_BUILDING_CHANCE;
          } while (nextForwardPosition < PLAYER_START.XValue && 
            specialChance == FUEL_BUILDING_CHANCE);
        }
        //Creates empty building spaces.
        if (specialChance < FUEL_BUILDING_CHANCE)
        {
          building = new EmptyBuilding(Vector2D(nextForwardPosition, 0.0f));
        }
        //Creates a fuel building.
        else if (specialChance == FUEL_BUILDING_CHANCE)
        {
          building = new RefuelBuilding(Vector2D(nextForwardPosition, 0.0f));
          //Places the camera in the right place at the very beginning of the game.
          if (m_needsRefuelBuilding)
          {
            MyDrawEngine::GetInstance()->theCamera.PlaceAt(m_player->GetPosition());
            m_player->PlaceAt(Vector2D(
              nextForwardPosition + (building->GetDimensions().XValue * 0.5f), 
              building->GetDimensions().YValue));
          }
          m_needsRefuelBuilding = false;
        }
        //Creates a normal building.
        else
        {
          //Randomises the building image.
          building = new Building(Vector2D(nextForwardPosition, 0.0f));
          //Chooses whether to spawn an enemy with the building.
          if (rand() % ENEMY_SPAWN_CHANCE < (int)m_difficulty)
          {
            SpawnEnemy(building, nextForwardPosition);
          }
          //Chooses whether to spawn a pickup.
          else if (rand() % PICKUP_CHANCE == 0)
          {
            SpawnPickup(building, nextForwardPosition);
          }
        }
        m_buildings.push_back(building->GetInfo());
        //Records last refuel building position.
        if (building->GetInfo().index == RefuelBuilding::REFUEL_BUILDING_INDEX)
        {
          m_distanceSinceLastRefuelBuilding = 0.0f;
        }
        else
        {
          m_distanceSinceLastRefuelBuilding += building->GetInfo().dimensions.XValue;
        }
      }
      //Creates a refuel building if one has been created there before.
      else if (m_buildings[m_nextForwardBuildingIndex].index == 
        RefuelBuilding::REFUEL_BUILDING_INDEX)
      {
        building = new RefuelBuilding(Vector2D(nextForwardPosition, 0.0f));
      }
      //Creates an empty building if one has been created there before.
      else if (m_buildings[m_nextForwardBuildingIndex].index ==
        EmptyBuilding::EMPTY_BUILDING_INDEX)
      {
        building = new EmptyBuilding(Vector2D(
          nextForwardPosition, 0.0f), m_buildings[m_nextForwardBuildingIndex].dimensions.XValue);
      }
      //Creates a normal building with the image of the one that was created in place before.
      else
      {
        building = new Building(Vector2D(nextForwardPosition, 0.0f), m_buildings[m_nextForwardBuildingIndex].index);
      }
      nextForwardPosition = building->GetInfo().xPos + (building->GetInfo().dimensions.XValue * 0.5f);
      Game::instance.m_objects.AddItem(building, true);
      ++m_nextForwardBuildingIndex; 
    }
  }

  void GameManager::GenerateBuildingsBehind()
  {
    if (m_nextBackBuildingIndex >= 0)
    {
      //Never creates new buildings, only loads new ones in from previously saved buildings.
      BuildingInfo info = m_buildings[m_nextBackBuildingIndex];
      float nextBackPosition = info.xPos - (info.dimensions.XValue * 0.5f);
      while (m_nextBackBuildingIndex >= 0 && nextBackPosition + DESPAWN_BUFFER_DISTANCE > 
        MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue)
      {
        Building* building = nullptr;
        //Creates a refuel building if one has been created there before.
        if (m_buildings[m_nextBackBuildingIndex].index == RefuelBuilding::REFUEL_BUILDING_INDEX)
        {
          building = new RefuelBuilding(Vector2D(nextBackPosition, 0.0f));
        }
        //Creates an empty building if one has been created there before.
        else if (m_buildings[m_nextBackBuildingIndex].index == EmptyBuilding::EMPTY_BUILDING_INDEX)
        {
          building = new EmptyBuilding(Vector2D(
            nextBackPosition, 0.0f), m_buildings[m_nextBackBuildingIndex].dimensions.XValue);
        }
        //Creates a normal building with the image of the one that was created in place before.
        else
        {
          building = new Building(Vector2D(
            nextBackPosition, 0.0f), m_buildings[m_nextBackBuildingIndex].index);
        }
        Game::instance.m_objects.AddItem(building, true);
        --m_nextBackBuildingIndex;
        if (m_nextBackBuildingIndex > 0)
        {
          BuildingInfo newInfo = m_buildings[m_nextBackBuildingIndex];
          nextBackPosition = newInfo.xPos - (newInfo.dimensions.XValue * 0.5f);
        }
      }
    }
  }

  void GameManager::SpawnEntities()
  {
    //Continuously checks what enemies or pickups should be present on the screen.
    for (unsigned int i = 0; i < m_entities.size(); ++i)
    {
      //Checks that the entity should be on the screen.
      if (!m_entities[i].spawned && !m_entities[i].removed &&
        m_entities[i].position.XValue + DESPAWN_BUFFER_DISTANCE > 
        MyDrawEngine::GetInstance()->GetViewport().GetBottomLeft().XValue &&
        m_entities[i].position.XValue - DESPAWN_BUFFER_DISTANCE < 
        MyDrawEngine::GetInstance()->GetViewport().GetBottomRight().XValue)
      {
        m_entities[i].spawned = true;
        switch (m_entities[i].entityType)
        {
        case ObjectType::HELICOPTER:
          Game::instance.m_objects.AddItem(new Helicopter(m_entities[i].position), true);
          break;
        case ObjectType::TURRET:
          Game::instance.m_objects.AddItem(new Turret(m_entities[i].position), true);
          break;
        case ObjectType::PICKUP:
          switch (m_entities[i].pickupType)
          {
          case PickupType::Speed:
            Game::instance.m_objects.AddItem(new SpeedBoostPickup(m_entities[i].position), true);
            break;
          case PickupType::Health:
            Game::instance.m_objects.AddItem(new HealthBoostPickup(m_entities[i].position), true);
            break;
          case PickupType::Life:
            Game::instance.m_objects.AddItem(new LifeBoostPickup(m_entities[i].position), true);
            break;
          case PickupType::Score:
            Game::instance.m_objects.AddItem(new ScoreBoostPickup(m_entities[i].position), true);
            break;
          case PickupType::Random:
            Game::instance.m_objects.AddItem(new RandomBoostPickup(m_entities[i].position), true);
            break;
          }
        }
      }
    }
  }

  void GameManager::RemoveEntity(float xPos, bool removed)
  {
    //Removes the entity from being eligible for spawning.
    for (unsigned int i = 0; i < m_entities.size(); ++i)
    {
      if (m_entities[i].position.XValue > xPos - 2.0f &&
        m_entities[i].position.XValue < xPos + 2.0f)
      {
        m_entities[i].spawned = false;
        m_entities[i].removed = removed;
      }
    }
  }

  void GameManager::Update(float frameTime)
  {
    GenerateBuildingsAhead();
    GenerateBuildingsBehind();
    SpawnEntities();
    //Updates the difficulty of the game as the player gets further.
    unsigned int oldDifficulty = m_difficulty;
    m_difficulty = (int)(m_player->GetPosition().XValue / DIFFICULTY_INCREASE_DISTANCE);
    //Spawns a boss round once every two difficulty increases.
    if (m_difficulty > oldDifficulty && m_difficulty > m_lastDifficultySpawnedAt && 
      m_difficulty % 2 == 0)
    {
      m_lastDifficultySpawnedAt = m_difficulty;
      for (unsigned int i = 0; i < 2 * m_difficulty; ++i)
      {
        Grinder* grinder = new Grinder(MyDrawEngine::GetInstance()->GetViewport().GetTopLeft());
        Game::instance.m_objects.AddItem(grinder, true);
      }
    }

    m_gravityTimer += frameTime;
    if (m_gravityTimer > 10.0f)
    {
      ResetGravity();
    }

    Vector2D cameraPosition = MyDrawEngine::GetInstance()->GetViewport().GetCentre();
    IncreaseParalaxOffset((cameraPosition - m_previousCameraPosition) * PARALAX_MULTIPLIER);
    m_previousCameraPosition = cameraPosition;
  }

  void GameManager::ResetGravity()
  {
    m_worldGravity.set(0, 1000.0f);
    m_gravityTimer = 0.0f;
  }

  Vector2D GameManager::GetPlayerSpawnPosition() const
  {
    Vector2D position = PLAYER_START;
    bool found = false;
    //Loops backwards through the buildings to find a refuel building behind the player.
    for (int i = (int)m_buildings.size() - 1; i >= 0 && !found; --i)
    {
      if (m_buildings[i].index == RefuelBuilding::REFUEL_BUILDING_INDEX && 
        m_buildings[i].xPos <= m_player->GetPosition().XValue + 1.0f)
      {
        position = Vector2D(m_buildings[i].xPos, m_buildings[i].dimensions.YValue);
        found = true;
      }
    }
    //If no refuel building could be found, use one ahead of the player.
    for (unsigned int i = 0; i < m_buildings.size() && !found; ++i)
    {
      if (m_buildings[i].index == RefuelBuilding::REFUEL_BUILDING_INDEX)
      {
        position = Vector2D(m_buildings[i].xPos, m_buildings[i].dimensions.YValue);
        found = true;
      }
    }
    return position;
  }

  void GameManager::ProcessCollision(GameObject& other) {}

  Circle2D& GameManager::GetBoundingCircle()
  {
    return m_boundingCircle;
  }

  std::vector<IShape2D*>& GameManager::GetCollisionShapes()
  {
    return m_collisionShapes;
  }

  Player& GameManager::GetPlayer()
  {
    return *m_player;
  }
}