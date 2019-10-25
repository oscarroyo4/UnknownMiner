#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "Animation.h"
#include "p2Point.h"

enum player_status {
	PLAYER_IDLE,
	PLAYER_FORWARD,
	PLAYER_BACKWARD,
	PLAYER_JUMP,
	PLAYER_IN_AIR,
	PLAYER_PUNCH,
	PLAYER_PUNCH_AIR,
	PLAYER_IN_PUNCH_FINISH,
	PLAYER_DEATH
};


class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

	// Called before render is available
	//bool Awake(pugi::xml_node&);

	// Called each loop iteration
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	bool OnGround();
	bool WallCollision();

	iPoint position;

	bool input = true;

private:
	bool LoadPlayer();
	bool ResetStates();

	int life;
	int speed;
	float airTimer;
	float deathTimer;
	fPoint vel;


	bool jumpEnable = true;
	bool punchEnable = true;
	bool punchAirEnable = true;
	bool dead = false;

	Uint32 punch_timer = 0;

	SDL_Texture* graphics;
	Animation* current_animation = &idle;
	Animation idle;
	Animation backward;
	Animation forward;
	Animation punch;
	Animation punch_air;
	Animation jump;
	Animation death;

	player_status status = PLAYER_IDLE;
	SDL_Rect r;
	Collider* colPlayer;
	Collider* colPlayerWalls;
	Collider* punchCol;
};
#endif //__j1PLAYER_H__