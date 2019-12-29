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

	//Rotate
	rotation.PushBack({ 1, 1, 16, 16 });
	rotation.PushBack({ 18, 1, 16, 16 });
	rotation.PushBack({ 35, 1, 16, 16 });
	rotation.PushBack({ 52, 1, 16, 16 });
	rotation.PushBack({ 69, 1, 16, 16 });
	rotation.PushBack({ 52, 1, 16, 16 });
	rotation.PushBack({ 35, 1, 16, 16 });
	rotation.PushBack({ 18, 1, 16, 16 });
	rotation.speed = 0.3f;
}

j1Coin::~j1Coin() {}

bool j1Coin::Awake(pugi::xml_node& config)
{
	bool ret = true;
	LOG("Loading coins from config_file");

	//Initial animation set
	current_animation = &rotation;

	//All diferent coins spawns
	initialPos1.x = config.child("initialPos1").attribute("x").as_int();
	initialPos1.y = config.child("initialPos1").attribute("y").as_int();
	initialPos2.x = config.child("initialPos2").attribute("x").as_int();
	initialPos2.y = config.child("initialPos2").attribute("y").as_int();
	initialPos3.x = config.child("initialPos3").attribute("x").as_int();
	initialPos3.y = config.child("initialPos3").attribute("y").as_int();
	initialPos4.x = config.child("initialPos4").attribute("x").as_int();
	initialPos4.y = config.child("initialPos4").attribute("y").as_int();
	initialPos5.x = config.child("initialPos5").attribute("x").as_int();
	initialPos5.y = config.child("initialPos5").attribute("y").as_int();
	initialPos6.x = config.child("initialPos6").attribute("x").as_int();
	initialPos6.y = config.child("initialPos6").attribute("y").as_int();

	return ret;
}

bool j1Coin::Start()
{
	bool ret = true;

	coin_tex = App->tex->Load("textures/Coin.png");
	r_collider = { position.x, position.y, 16, 16 };
	colCoin = App->collision->AddCollider(r_collider, COLLIDER_COIN);

	return ret;
}

bool j1Coin::PreUpdate()
{
	bool ret = true;

	if (colCoin->CheckCollision(App->scene->player->r_collider) && !collected)
	{
		App->scene->coins++;
		LOG("Coin collected");
		collected = true;
	}
	
	return ret;
}

bool j1Coin::Update(float dt)
{
	bool ret = true;
	
	if (coin_tex != nullptr) {
		r = current_animation->GetCurrentFrame(dt);
		App->render->Blit(coin_tex, position.x, position.y, &r);
	}

	colCoin->SetPos(position.x, position.y);

	r.x = position.x;
	r.y = position.y;

	return ret;
}

bool j1Coin::PostUpdate() {
	return true;
}

bool j1Coin::CleanUp()
{
	bool ret = true;
	
	if (coin_tex != nullptr) App->tex->UnLoad(coin_tex);
	colCoin->to_delete;

	return ret;
}