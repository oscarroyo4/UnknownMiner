#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "Animation.h"
#include "p2Point.h"

enum player_status {
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_INJUMPFINISH
};


class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called each loop iteration
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

private:
	iPoint position;
	int life;
	bool input = true;
	bool jumpEnable = true;
	SDL_Texture* graphics;
	Animation* current_animation = &idle;
	Animation idle;
	Animation walk;
	float jump_timer = 0;
	player_status status = PLAYER_IDLE;
};
#endif //__j1PLAYER_H__