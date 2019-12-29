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
	name.create("coin");

	coin_tex = nullptr;

	//Rotate animation
	rotation.PushBack({ 20, 20, 160, 160 });
	/*rotation.PushBack({ 30, 0, 21, 26 });
	rotation.PushBack({ 63, 0, 14, 26 });
	rotation.PushBack({ 96, 0, 8, 26 });
	rotation.PushBack({ 124, 0, 14, 26 });
	rotation.PushBack({ 151, 0, 21, 26 });*/
	rotation.speed = 0.2f;
}

j1Coin::~j1Coin() {}

bool j1Coin::Awake(pugi::xml_node& config)
{
	bool ret = true;

	//Initialize variables from j1Coin.h
	current_animation = &rotation;

	position.x = App->scene->player->position.x +50;
	position.y = App->scene->player->position.y +20;
	coin_position = 0;

	CoinFX = "audio/fx/Coin.wav";

	return ret;
}

bool j1Coin::Start()
{
	bool ret = true;

	coin_tex = App->tex->Load("textures/Coin.png");
	App->audio->LoadFx(CoinFX.GetString());

	r_collider = { position.x, position.y + 2, 10, 10 };
	colCoin = App->collision->AddCollider(r_collider, COLLIDER_COIN);

	return ret;
}

bool j1Coin::PreUpdate()
{
	bool ret = true;

	if (colCoin->CheckCollision(App->scene->player->r_collider))
	{
		coin_collected = true;
	}
	
	return ret;
}

bool j1Coin::Update(float dt)
{
	bool ret = true;

	App->render->Blit(coin_tex, position.x, position.y, &r);

	if (coin_collected == true)
	{
		CollectCoin();
		if (!coinSound) { coinSound = true; App->audio->PlayFx(coinFx, 0); }
		
	}
	
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
	if (colCoin->CheckCollision(App->scene->player->r_collider)) 
	{
		collected_coins++;
		LOG("COIN");
	}

	/*switch (c2->type)
	{
	case COLLIDER_PLAYER:

		App->entitymanager->entities->Delete;
		//colCoin->to_delete;
		collected_coins++;
		LOG("COIN");

		break;
	default:
		break;
	}*/
}

void j1Coin::CollectCoin() 
{
	
	switch (coin_position)
	{
	case 0:
		collected_coins = 1;
		colCoin->SetPos(position.x + 100, position.y - 50);
		break;

	case 1:	
		collected_coins = 2;
		colCoin->SetPos(position.x + 250, position.y - 120);
		break;

	default:
		break;
	}
	coin_position++;

}