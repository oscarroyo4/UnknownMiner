#ifndef _ENTITY_H
#define _ENTITY_H

#include "j1Module.h"
#include "p2Point.h"

#define DT_CORRECT 30

enum class Types
{
	none = 0,
	player,
	enemy_ground,
	enemy_air,
	coin,
	unknown
};



class Entity : public j1Module
{
public:
	Entity(Types type) {

	}

	// Destructor
	virtual ~Entity() {

	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called el pepe each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	//Called when loading the game
	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	//Called when saving the game
	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

public:
	Types entity_type;
	iPoint position;
	fPoint vel;
	SDL_Rect r;
	SDL_Rect r_collider;
	bool godmode = false;
	bool input = true;
	bool isHit = false;
	bool collected = false;

	int life;

	iPoint initialPos1;
	iPoint initialPos2;
	iPoint initialPos3;
	iPoint initialPos4;
	iPoint initialPos5;
	iPoint initialPos6;
};

#endif // !ENTITY_H