#ifndef __j1GROUNDENEMY_H__
#define __j1GROUNDENEMY_H__

#include "j1Module.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "Animation.h"
#include "p2Point.h"
#include "Entity.h"

enum groundEnemy_status {
	GROUNDENEMY_IDLE,
	GROUNDENEMY_IN_AIR,
	GROUNDENEMY_MOVE,
	GROUNDENEMY_ATTACK,
	GROUNDENEMY_ATTACK_FINISH,
	GROUNDENEMY_DEATH
};


class j1GroundEnemy : public Entity
{
public:

	j1GroundEnemy();

	// Destructor
	virtual ~j1GroundEnemy();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called each loop iteration
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called each loop iteration
	void Draw();

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

	int initialX;
	int initialY;
	int initialX2;
	int initialY2;

private:

	bool ResetStates();

	int life;
	int speed;
	float gravity;
	int deathLimit;
	fPoint vel;

	bool atackEnable = true;
	bool dead = false;

	Uint32 punch_timer = 0;

	p2SString texPath;
	p2SString hitPath;
	p2SString movePath;
	p2SString attackPath;
	SDL_Texture* graphics;
	int hitFx;
	int moveFx;
	int attackFx;
	int punchTime;

	int pathSteps = 0;
	iPoint nextPos;

	Animation* current_animation = &idle;
	Animation idle;
	Animation move;
	Animation attack;
	Animation death;

	groundEnemy_status status = GROUNDENEMY_IN_AIR;
	SDL_Rect r;
	Collider* colGroundEnemy;
};
#endif //__j1GROUNDENEMY_H__
