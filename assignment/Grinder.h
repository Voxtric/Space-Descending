#pragma once

#include "Enemy.h"

namespace Voxtric
{
  class Grinder : public Enemy
  {
  public:
    Grinder(Vector2D position);

    void Update(float frameTime) override;
    void ProcessCollision(GameObject& other) override;
    void OnPause() override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

  private:
    //How many sounds that can be played at once.
	  static const unsigned int SIMULTANEOUS_SOUNDS = 10;      
    static const unsigned int SCORE_MODIFIER = 1000;  //Score increase when destroyed.
    //How long before a new target position is chosen when stationary.
    static const unsigned int PLAYER_CHASE_WAIT_MILISECONDS = 1000; 
    //Maximum distance from the player a target position can be chosen.
    static const unsigned int MAX_DISTANCE_FROM_PLAYER = 900;
    //Maximum number of attempts to find a new target position that can be made per frame.
    static const unsigned int MAX_NEW_POSITION_ATTEMPTS = 5;
    static const unsigned int NO_ACTIVE_SOUND = -1;  //Indicates there is no sound playing.
    static const float WAIT_PER_MOVE; //How long the grinder should wait per move.
	  static const float MOVE_SPEED;    //Speed at which the grinder should move.
    static const float PLAYER_DAMAGE; //How much to damage the player by per collision.
    static const float RADIUS;        //Radius of the sprite.

	  static SoundIndex s_grindSounds[SIMULTANEOUS_SOUNDS]; //All grind sounds.
    static unsigned int s_grindSoundIndex;                //Current grind sound array index.
    static SoundIndex s_backgroundSound;  //Only played when there is a live grinder.
    static unsigned int s_instances;      //Number of grinders alive.
    //Whether the grinder background sound is currently playing.
    static bool s_backgroundSoundPlaying;

    float m_timeSincePlayerChase;
    Vector2D m_targetPosition;
    float m_spin;
    int m_activeSoundIndex;

    void TryDespawn() override;
    
    Vector2D GetNewTargetPosition() const;  //Gets a new position to move towards.
    //Checks that the path to the target position is clear.
    bool PathClear(const Segment2D& path) const; 
  };
}