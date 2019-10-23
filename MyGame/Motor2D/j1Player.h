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
	PLAYER_JUMP_FORWARD,
	PLAYER_IN_AIR,
	PLAYER_PUNCH,
	PLAYER_IN_PUNCH_FINISH
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


	iPoint position;
private:
	bool LoadPlayer();

	int life;
	int speed;
	iPoint vel;

	bool input = true;
	bool jumpEnable = true;
	bool punchEnable = true;

	Uint32 punch_timer = 0;

	SDL_Texture* graphics;
	Animation* current_animation = &idle;
	Animation idle;
	Animation backward;
	Animation forward;
	Animation punch;
	Animation jump;

	player_status status = PLAYER_IDLE;
	SDL_Rect r;
	Collider* colPlayer;
	Collider* punchCol;
};
#endif //__j1PLAYER_H__