#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "Animation.h"
#include "p2Point.h"
#include "Entity.h"

enum player_status {
	PLAYER_IDLE,
	PLAYER_FORWARD,
	PLAYER_BACKWARD,
	PLAYER_JUMP,
	PLAYER_IN_AIR,
	PLAYER_PUNCH,
	PLAYER_PUNCH_AIR,
	PLAYER_IN_PUNCH_FINISH,
	PLAYER_HIT,
	PLAYER_DEATH
};


class j1Player : public Entity
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

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

	// Collisions
	bool OnGround();
	bool WallCollision();

	//Public variables
	//iPoint position;

	Collider* colPlayer;

private:

	bool ResetStates();


	int speed;
	float gravity;
	int deathLimit;
	float deathTimer_config;
	float airTimer;
	float deathTimer;

	bool jumpEnable = true;
	bool punchEnable = true;
	bool punchAirEnable = true;
	bool dead = false;
	bool godmode = false;
	bool lookforward = true;
	bool punchHit = false;

	Uint32 punch_timer = 0;
	Uint32 punchair_timer = 0;
	Uint32 hit_timer = 0;

	p2SString texPath;
	p2SString swoshPath;
	p2SString hitPath;
	p2SString jumpPath;
	p2SString landPath;
	SDL_Texture* graphics;
	int swoshFx;
	int hitFx;
	int jumpFx;
	int landFx;
	uint punchTime;

	Animation* current_animation = &idle;
	Animation idle;
	Animation backward;
	Animation forward;
	Animation punch;
	Animation punch_air;
	Animation jump;
	Animation death;

	player_status status = PLAYER_IDLE;

	Collider* colPlayerWalls;
	Collider* punchCol;
	SDL_Rect r;
};
#endif //__j1PLAYER_H__