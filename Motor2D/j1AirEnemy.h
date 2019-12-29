#ifndef __j1AIRENEMY_H__
#define __j1AIRENEMY_H__

#include "j1Module.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "Animation.h"
#include "p2Point.h"
#include "Entity.h"

enum airEnemy_status {
	AIRENEMY_IDLE,
	AIRENEMY_FLY,
	AIRENEMY_HIT,
	AIRENEMY_ATTACK,
	AIRENEMY_ATTACK_FINISH,
	AIRENEMY_DEATH
};


class j1AirEnemy : public Entity
{
public:

	j1AirEnemy();

	// Destructor
	virtual ~j1AirEnemy();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called each loop iteration
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	void Draw(float dt);

	// Called before quitting
	bool CleanUp();
	bool Disable();

	// Load / Save
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	// Collisions
	bool OnGround();
	bool WallCollision();

private:

	bool ResetStates();

	int speed;
	float gravity;
	int deathLimit;
	float hit_delay;

	bool flip;
	bool attackEnable = true;
	bool dead = false;

	Uint32 hit_timer = 0;

	p2SString texPath;
	p2SString hitPath;
	p2SString flyPath;
	p2SString attackPath;
	p2SString deathPath;
	SDL_Texture* graphics;
	int hitFx;
	int flyFx;
	int attackFx;
	int deathFx;
	int hitTime;

	int pathSteps = 0;
	iPoint nextPos;

	Animation* current_animation = &idle;
	Animation idle;
	Animation fly;
	Animation attack;
	Animation death;

	airEnemy_status status = AIRENEMY_IDLE;
	Collider* colAirEnemy;
};
#endif //__j1AIRENEMY_H__