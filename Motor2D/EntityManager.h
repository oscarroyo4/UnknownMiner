#ifndef _ENTITYMANAGER_H
#define _ENTITYMANAGER_H

#include "j1Module.h"
#include "Entity.h"
#include "j1Player.h"
#include "j1AirEnemy.h"
#include "j1GroundEnemy.h"
#include "p2List.h"
#include "PugiXml\src\pugixml.hpp"

class Entity;

class EntityManager : public j1Module
{
public:

	EntityManager();

	// Destructor
	~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	//Called when loading the game
	bool Load(pugi::xml_node&);

	//Called when saving the game
	bool Save(pugi::xml_node&) const;

	//Called when creating a new Entity
	Entity* CreateEntity(Types type);

	//Called when deleting a new Entity
	bool DeleteEntity(Entity*);

public:
	pugi::xml_node node;
	p2List <Entity*> entities;
	//Entity* playerEntity;
};
#endif // !_ENTITYMANAGER_H