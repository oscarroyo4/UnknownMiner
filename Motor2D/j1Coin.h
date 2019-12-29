#ifndef _j1COIN_H
#define _j1COIN_H

#include "j1Module.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "Animation.h"
#include "p2Point.h"
#include "Entity.h"

class j1Coin : public Entity
{
public:
	j1Coin();
	~j1Coin();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:
	Animation*		current_animation = &rotation;
	Animation		rotation;
	SDL_Texture*	coin_tex;
	Collider*		colCoin;
};

#endif // !_j1COIN_H