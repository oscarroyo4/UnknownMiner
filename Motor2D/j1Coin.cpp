#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1FadeToBlack.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1PathFinding.h"
#include "Animation.h"
#include "j1Coin.h"
#include "Entity.h"
#include "EntityManager.h"
#include <math.h>

j1Coin::j1Coin() : Entity(Types::coin)
{
	//type = Types::coin;

	coin_tex = nullptr;

	float speed = 0.1f;
	//Rotate
	rotation.PushBack({ 20, 20, 160, 160 });
	/*rotation.PushBack({ 30, 0, 21, 26 });
	rotation.PushBack({ 63, 0, 14, 26 });
	rotation.PushBack({ 96, 0, 8, 26 });
	rotation.PushBack({ 124, 0, 14, 26 });
	rotation.PushBack({ 151, 0, 21, 26 });*/
}

j1Coin::~j1Coin() {}

bool j1Coin::Awake(pugi::xml_node& config)
{
	bool ret = true;

	//Initialize variables from j1Coin.h
	current_animation = &rotation;

	position.x = App->scene->player->position.x +20;
	position.y = App->scene->player->position.y +20;

	return ret;
}

bool j1Coin::Start()
{
	bool ret = true;

	coin_tex = App->tex->Load("textures/Coin.png");

	r_collider = { position.x, position.y + 2, 10, 10 };
	colCoin = App->collision->AddCollider(r_collider, COLLIDER_COIN);

	return ret;
}

bool j1Coin::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool j1Coin::Update(float dt)
{
	bool ret = true;

	App->render->Blit(coin_tex, position.x, position.y, &r);

	return ret;
}

bool j1Coin::PostUpdate()
{
	bool ret = true;

	return ret;
}

bool j1Coin::CleanUp()
{
	bool ret = true;

	coin_tex = nullptr;
	colCoin->to_delete;

	return ret;
}

void j1Coin::OnCollision(Collider* c1, Collider* c2)
{
	switch (c2->type)
	{
	case COLLIDER_PLAYER:

		coin_tex = nullptr;
		colCoin->to_delete;
		collected_coins++;
		LOG("COIN");

		break;
	default:
		break;
	}
}