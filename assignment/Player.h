#pragma once

#include "GameObject.h"
#include "mysoundengine.h"
#include "myinputs.h"

namespace Voxtric
{
	class Player : public GameObject
  {
  public:
    static const unsigned int MAX_LIVES = 3;  //Maximum amount of lives.
    static const unsigned int MAX_AMMO = 25;  //Maximum amount of ammo.
    static const float MAX_HEALTH;            //Maximum amount of health.
    static const float MAX_FUEL;              //maximum amount of fuel.
    static const float FRICTION;              //Player friction.

    Player(Vector2D position);

    void BoostSpeed();  //Increases speed.
    void BoostHealth(); //Increases health.
    void BoostLives();  //Increases lives.
    void InvertControls();  

    void Damage(float damage);  //Deals the player damage.
    void PlaceAt(Vector2D position);  //Places the player at given position.
    void Refuel();  //Increase fuel and ammo.
    void AddScore(unsigned int score);  //Increases score.
    void AddExplosionForce(Vector2D force); //Damages and moves the player.
    
    void Update(float frameTime) override;
    void OnPause() override;
    void ProcessCollision(GameObject& other) override;
    Circle2D& GetBoundingCircle() override;
    std::vector<IShape2D*>& GetCollisionShapes() override;

    unsigned int GetAmmo() const;
    float GetFuel() const;
    int GetScore() const;
    float GetHealth() const;
    unsigned int GetLives() const;
    bool CanRefuel() const;
    int GetHighestDistanceScore() const;
    Vector2D GetPosition() const;


  private:
    //Number of collision segments used for the collision shape.
    static const unsigned int COLLISION_SEGMENTS_COUNT = 4;
    //Velocity magnitude that won't result in damage on collision. 
    static const float COLLISION_THRESHOLD;
    static const float COLLIDED_TURN_WAIT;  //Delay before player can turn after collision.

    //Number of sounds that can be played at once.
    static const unsigned int SIMULTANEOUS_SOUNDS = 3;

    static const float PLAYER_RESET_YVALUE; //Value under the screen to hide the player.
    static const float PLAYER_RESET_WAIT; //Duration the player should wait before resetting.
    static const float LOW_HEALTH_THRESHOLD;  //Point at which low health sounds should play.

    static const float ACCELERATION;  //Acceleration when forward is held.
    static const float TURN_SPEED;  //Speed at which the player should turn.
	  static const float NO_TARGET; //Value marking no rotation target.

    static const float HEALTH_REDUCTION_MODIFIER; //Reduces damage reductions.
    static const float FUEL_INCREMENT;  //How much to increase fuel with each refuel.

    static const float BOOST_DURATION;  //How long any pickup boost should last.
    //How much faster a speed boost pickup should make the player.
    static const float BOOST_SPEED_MODIFIER; 

    static const float WAIT_TIME_PER_SHOT;  //Delay between each shot.
    static const float PROJECTILE_SHOOT_SPEED;  //Speed that projectils should travel at.

    static const float CAMERA_BACK_CONSTRAINT;  //Where the camera can go no further back.
    static const float CAMERA_UP_CONSTRAINT;    //Where the camera can go no higher.
    static const float CAMERA_DOWN_CONSTRAINT;  //Where the camera can go no lower.
    static const float PLAYER_BACK_CONSTRAINT;  //Where the player can go no further back.

    static const float THRUST_BOTTOM; //Bottom thruster position.
    static const float THRUST_SIDE;   //Side thruster position.

    static const float EXPLOSION_SCALE; //Size of the explosion.
    static const float EXPLOSION_SPEED; //Speed of the explosion/
    static const float EXPLOSION_PARTICLES_DURATION;  //Duration of the explosion particle emitter.
    //Particles made by the explosion every second.
    static const unsigned int EXPLOSION_PARTICLES_PER_SECOND = 150;
    //Force with which to fling the particles outwards.
    static const unsigned int EXPLOSION_PARTICLES_FORCE = 600;
    static const float THRUSTER_SCALE;  //Size of the thruster effects.
    static const float THRUSTER_SPEED;  //Speed of the thruster effects.
    static const unsigned int THRUSTER_START_FRAME = 5;//Starting frame for the thruster animation.
    static const unsigned int THRUST_VOLUME_MINIMUM = 3000; //Minimum volume of thrusting sound.
    static const float THRUST_VOLUME_MODIFIER;  //Change to the thruster sound volume.
	  static const float THRUST_FUEL_COST;  //Fuel cost of thrusting.

    static const float LEFT_TILT_ANGLE; //Furthest left tilt before not landing.
    static const float RIGHT_TILT_ANGLE;  //Furthest right tilt before not landing.
    static const float QUARTER_TURN_ANGLE;  //90 degrees in radians.

    static SoundIndex s_buildingCollisionSounds[SIMULTANEOUS_SOUNDS]; 
    static unsigned int s_buildingCollisionSoundIndex; 
    static SoundIndex s_engineSound;
    static SoundIndex s_healthLowSound;

    Segment2D m_collisionSegments[COLLISION_SEGMENTS_COUNT];
    Vector2D m_dimensions;
    Vector2D m_velocity;
    
    float m_targetXPosition;
    float m_originalDistanceToTarget;
    float m_targetAngle;
    unsigned long m_framesElapsed;

    float m_health;
    float m_fuel;
    unsigned int m_ammo;
    unsigned int m_lives;
    bool m_needsReset;
    
    int m_highestScore;
    unsigned int m_additionalScore;
    
    float m_lastFrameTime;
    float m_timeSinceCollision;
    float m_timeSinceLastShot;
    float m_timeUntilReset;
    
    bool m_engineSoundPlaying;
    bool m_healthLowSoundPlaying;

    float m_boostDuration;
    float m_extraAcceleration;
    bool m_controlsInverted;

    void HandleRotation(MyInputs* input, float frameTime);  //Handles player input for rotation.
    //handles player input for position changes.
    void HandleTranslation(MyInputs* input, float frameTime, bool disableThrust);
    void HandleHealth();  //Handles effects of health changes.
    void HandleReset(float frameTime);  //Handles the resetting of the player on death.

    void PlaceCamera(bool& disableThrust) const;  //Places the camera at an appropriate location.
    //Moves the player out of the building it is colliding with.
    void MovePlayerOutCollision(const Segment2D* playerColShape,
      const Rectangle2D* buildingColShape, Vector2D collisionNormal);
    //Determines which of the different collision shapes that make up the entire collision
    //actually contributed to the collision. Values are returned through parameters.
    void FindCollisionObjects(GameObject& other, 
      Segment2D*& playerColShape, Rectangle2D*& buildingColShape);
    void DamageShip(const Segment2D* playerColShape, const Rectangle2D* buildingColShape);
	};
}