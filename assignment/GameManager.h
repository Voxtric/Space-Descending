#pragma once

#include "Player.h"
#include "GameObject.h"
#include <map>
#include "BuildingInfo.hpp"
#include "Building.h"
#include "EntityInfo.hpp"

namespace Voxtric
{
  class GameManager : public GameObject
  {
  public:
    static const unsigned int SCORES_TO_SAVE = 5; //How many high scores can be saved.
    //The range of space that a plane can spawn at.
    static const unsigned int PLANE_SPAWN_HEIGHT_RANGE = 600;

    static const unsigned int SPECIAL_BUILDING_CHANCE = 15;//Chance of spawning a special building.
    static const unsigned int FUEL_BUILDING_CHANCE = 3; //Chance of spawning a fuel building.
    static const unsigned int PICKUP_CHANCE = 30; //Chance of spawning a pickup.
    static const unsigned int ENEMY_SPAWN_CHANCE = 10;  //Chance of spawning an enemy.

    static const Vector2D PLAYER_START; //Default player spawn position (Rarely used).
    static const float DIFFICULTY_INCREASE_DISTANCE;  //Distance between each difficulty increase.
    //Distance off either side of the screen before entity despawns.
    static const float DESPAWN_BUFFER_DISTANCE; 
    static const float TOP_BUILDING_HEIGHT; //Height necessary to avoid tallest building.
    static const float PARALAX_MULTIPLIER;  //Movement multiplier to make paralaz scrolling work.

    GameManager();

    void StartGame(); //Initialises game.
    Player& GetPlayer();  //Returns the player.
    void UpdateHighScores();  //Updates the high score list.
    void IncreaseParalaxOffset(Vector2D increase);  //Increases the paralax offset.
    void ReduceForwardBuildingIndex();  //Monitors different building for spawning forward.
    void IncreaseBackBuildingIndex(); //Monitors different building for spawning back.
    void RemoveEntity(float position, bool dead); //Removes enemy or pickup after exiting screen.
    void RandomiseGravity();  //Randomises world gravity.
    void ResetGravity();  //Resets the gravity.

    Vector2D GetPlayerSpawnPosition() const; 
    const int* GetHighScores() const;
    Vector2D GetParalaxOffset() const;
    Vector2D GetGravity() const;

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;
    void Draw() override;

  private:
    //Maximum distance before a refuel building must be spawned.
    static const float MAX_DISTANCE_PER_REFUEL_BUILDING;  

    Player* m_player;
    unsigned int m_difficulty;
    int m_highScores[SCORES_TO_SAVE];
    Vector2D m_paralaxOffset;
    Vector2D m_previousCameraPosition;
    bool m_needsRefuelBuilding;
    float m_distanceSinceLastRefuelBuilding;
    unsigned int m_lastDifficultySpawnedAt;
    Vector2D m_worldGravity;
    float m_gravityTimer;

    std::vector<EntityInfo> m_entities;
    std::vector<BuildingInfo> m_buildings;
    int m_nextForwardBuildingIndex;
    int m_nextBackBuildingIndex;

    void GenerateBuildingsAhead();  //Generates buildings ahead of the camera.
    void GenerateBuildingsBehind(); //Generates buildings behind the camera.
    void SpawnEntities(); //Spawns pickups and enemies before or after the camera.
    void SpawnEnemy(const Building* building, float nextForwardPosition); //Spawns a random enemy.
    void SpawnPickup(const Building* building, float nextForwardPosition);//Spawns a random pickup.
  };
}