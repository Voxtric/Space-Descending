
#include "ObjectManager.h"
#include <algorithm>


ObjectManager::ObjectManager()
{
	m_timer.setMinimumFrameTime(0.01);
	m_timer.mark();
	m_timer.mark();
  m_currentScene = 0;
}

ObjectManager::~ObjectManager()
{
	DeleteAllObjects();
}

bool ObjectManager::ShapeCollides(const IShape2D* shape, 
  std::vector<GameObject*>& collisions) const
{
  bool collision = false;
  collisions.clear();
  auto it1 = m_colliderList.begin();
  for (; it1 != m_colliderList.end(); ++it1)
  {
    if ((*it1)->IsActive() && (*it1)->GetSceneNumber() == m_currentScene)
    {
      if ((*it1)->HasCollided((IShape2D*)shape))
      {
        collision = true;
        collisions.emplace_back(*it1);
      }
    }
  }
  return collision;
}

void ObjectManager::AddItem(GameObject* pNewItem,  bool collides)
{
	// Find position of first item with a higher or equal Draw Depth

	auto it = m_allObjectList.begin();

	while(it!=m_allObjectList.end() &&(*it)->GetDrawDepth() < pNewItem->GetDrawDepth() )
	{
		++it;
	}
	m_allObjectList.insert(it, pNewItem);

   if(collides)
	{
		m_colliderList.push_back(pNewItem);
	}
}

void ObjectManager::DeleteInactiveItems()
{

	// Remove all inactive objects from collider list

	auto itc = std::remove_if( m_colliderList.begin(), m_colliderList.end(), [](GameObject* pGO){ return !pGO->IsActive();} ) ;
	m_colliderList.erase(  itc, m_colliderList.end());

	// Delete all inactive objects
	auto it = m_allObjectList.begin();

	for( ;it!=m_allObjectList.end();++it)
	{
		if(!(*it)->IsActive())
		{
			delete *it;
			*it = nullptr;
		}
	}

	// Remove all inactive objects from master list
	auto ita = std::remove( m_allObjectList.begin(), m_allObjectList.end(), nullptr ) ;
	m_allObjectList.erase(  ita, m_allObjectList.end());

}

void ObjectManager::OnPauseAll()
{
  auto it = m_allObjectList.begin();
  for (; it != m_allObjectList.end(); ++it)
  {
    if ((*it)->IsActive() && (*it)->GetSceneNumber() == m_currentScene)
    {
      (*it)->OnPause();
    }
  }
}

void ObjectManager::UpdateAll()
{
	m_timer.mark();

	// Limit frame rate to 20 fps.
	if(m_timer.mdFrameTime>0.05)
	{
		m_timer.mdFrameTime=0.01;
	}
	auto it = m_allObjectList.begin();

	for( ;it!=m_allObjectList.end();++it)
	{
    if ((*it)->IsActive() && (*it)->GetSceneNumber() == m_currentScene)
		{
			(*it)->Update(float(m_timer.mdFrameTime));
		}
	}
}

void ObjectManager::DrawAll()
{
	auto it = m_allObjectList.begin();

	for( ;it!=m_allObjectList.end();++it)
	{
    if ((*it)->IsActive() && (*it)->GetSceneNumber() == m_currentScene)
		{
			(*it)->Draw();
		}
	}
}

void ObjectManager::ProcessCollisions()
{
	//  collisions
	auto it1 = m_colliderList.begin();

	for( ;it1!=m_colliderList.end();++it1)
	{
		auto it2 = it1;
		it2++;

		for( ;it2!=m_colliderList.end();++it2)
		{
			if((*it1)->IsActive() && (*it2)->IsActive()
        && (*it1)->GetSceneNumber() == m_currentScene
        && (*it2)->GetSceneNumber() == m_currentScene)
			{
				if( (*it1)->HasCollided(**it2) )
				{
					(*it1)->ProcessCollision(**it2);
					(*it2)->ProcessCollision(**it1);
				}
			}
		}
	}
}


int ObjectManager::GetNumObjects() const
{
	return m_allObjectList.size();
}

void ObjectManager::DeleteAllObjects()
{
	auto it = m_allObjectList.begin();

	for( ;it!=m_allObjectList.end();++it)
	{
		delete *it;
	}

	m_allObjectList.clear();
	m_colliderList.clear();
}

void ObjectManager::HandleEvent(Event evt)
{

	auto it = m_allObjectList.begin();

	for( ;it!=m_allObjectList.end();++it)
	{
    if ((*it)->m_handleEvents && (*it)->IsActive()
      && (*it)->GetSceneNumber() == m_currentScene)
			(*it)->HandleEvent(evt);
	}
}

// Sets the current scene used by the object system
// Only objects in the current scene will be drawn or updated.
void ObjectManager::SetCurrentScene(int sceneNumber)
{
  m_currentScene = sceneNumber;
}

// Returns the number of the current scene.
int ObjectManager::GetCurrentScene()
{
  return m_currentScene;
}

// Sets ALL objects to inactive.
void ObjectManager::DeactivateAll()
{
  auto it = m_allObjectList.begin();

  for (; it != m_allObjectList.end(); ++it)
  {
    (*it)->Deactivate();
  }
}

// Deactivates all objects with the corresponding type
void ObjectManager::DeactivateType(ObjectType type)
{
  auto it = m_allObjectList.begin();

  for (; it != m_allObjectList.end(); ++it)
  {
    if ((*it)->GetType() == type)
      (*it)->Deactivate();
  }
}

// Deactivates all objects with the specified scene
void ObjectManager::DeactivateScene(int sceneNumber)
{
  auto it = m_allObjectList.begin();

  for (; it != m_allObjectList.end(); ++it)
  {
    if ((*it)->GetSceneNumber() == sceneNumber)
      (*it)->Deactivate();
  }
}