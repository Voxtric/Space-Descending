#pragma once

#include "GameObject.h"
#include <list>
#include "gametimer.h"

class ObjectManager
{
private:
	std::list<GameObject*> m_allObjectList;
	std::list<GameObject*> m_colliderList;
	GameTimer m_timer;
  int m_currentScene;
public:
  // Sets up the object and sets the currentScene to zero
  // Also sets the gametimer
	ObjectManager();

  // Deletes all objects
	~ObjectManager();

  // Adds a new item to the list of objects.
  // Note: 
  // This function does not set the GameObject's scene
  // Parameters:
  //  pNewItem - Pointer to the object to be added
  //  collides - If true, this object will also be
  //          added to the list of collidable objects
	void AddItem(GameObject* pNewItem, bool collides);

  // Returns the number of objects currently in the Manager
  // Returns:
  //   The number of objects  in all scenes (including disabled objects 
  //    that have not been deleted)
	int GetNumObjects() const;

  // Deletes all objects in all scenes that have been set to "disabled".
	void DeleteInactiveItems();

  // Updates all objects in the current scene.
	void UpdateAll();

  // OnPauses all objects in the current scene.
  void OnPauseAll();

  // Draws all objects in the current scene
	void DrawAll();

  // Checks for collisions between all objects in the current scene.
  // If any collide, ProcessCollision will be called for both objects
	void ProcessCollisions();

  // Deletes all objects in all scenes
	void DeleteAllObjects();			// Are you sure? (Y/N)

  // Sends the event to all objects that are set to receive events
  // Note: this is actually a fairly inefficient system at present,
  // but it is intended more for convenience than efficiency.
  // Note: Only active objects in the current scene will receive the event
  // Parameters:
  //    evt - the event to be sent to all objects
	void HandleEvent(Event evt);

  // Sets the current scene used by the object system
  // Only objects in the current scene will be drawn or updated.
  // Parameters:
  //  sceneNumber - the number of the scene which will become the current scene
  void SetCurrentScene(int sceneNumber);

  // Returns the number of the current scene.
  // Returns:
  //  the number of the current scene.
  int GetCurrentScene();

  // Sets ALL objects to inactive.
  void DeactivateAll();

  // Deactivates all objects with the corresponding type
  // Parameters:
  //    type - The type of object to be deleted
  void DeactivateType(ObjectType type);

  // Deactivates all objects with the specified scene
  // Parameters:
  //    sceneNumber - The number of the scene which objects 
  //            will be deleted from
  void DeactivateScene(int sceneNumber);

  bool ShapeCollides(const IShape2D* shape, std::vector<GameObject*>& collisions) const;
};
