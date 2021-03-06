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
#include "j1AirEnemy.h"
#include "Entity.h"
#include "EntityManager.h"
#include <math.h>

j1AirEnemy::j1AirEnemy() : Entity(Types::enemy_air)
{
	name.create("airenemy");

	//animations
	idle.PushBack({ 0, 0, 16, 16 });
	idle.PushBack({ 16, 0, 16, 16 });
	idle.PushBack({ 32, 0, 16, 16 });
	idle.PushBack({ 48, 0, 16, 16 });
	idle.PushBack({ 64, 0, 16, 16 });
	idle.speed = 0.2f;
	
	fly.PushBack({ 0, 16, 16, 16 });
	fly.PushBack({ 16, 16, 16, 16 });
	fly.PushBack({ 32, 16, 16, 16 });
	fly.PushBack({ 48, 16, 16, 16 });
	fly.PushBack({ 64, 16, 16, 16 });
	fly.speed = 0.2f;

	death.PushBack({ 0, 32, 16, 16 });
	death.PushBack({ 16, 32, 16, 16 });
	death.PushBack({ 32, 32, 16, 16 });
	death.PushBack({ 48, 32, 16, 16 });
	death.PushBack({ 64, 32, 16, 16 });
	death.speed = 0.2f;
	death.loop = false;
	/*
	attack.PushBack({ 0, 32, 32, 32 });
	attack.PushBack({ 32, 32, 32, 32 });
	attack.PushBack({ 64, 32, 32, 32 });
	attack.PushBack({ 96, 32, 32, 32 });
	attack.speed = 0.2f;
	*/
}

// Destructor
j1AirEnemy::~j1AirEnemy()
{}

bool j1AirEnemy::Awake(pugi::xml_node& config) {
	bool ret = true;
	LOG("Loading air enemy from config_file");

	texPath = config.child("path").attribute("tex").as_string();
	hitPath = config.child("path").attribute("hit").as_string();
	flyPath = config.child("path").attribute("fly").as_string();
	attackPath = config.child("path").attribute("attack").as_string();
	life = config.child("propierties").attribute("life").as_int();
	speed = config.child("propierties").attribute("speed").as_int();
	gravity = config.child("propierties").attribute("gravity").as_float();
	hitTime = config.child("propierties").attribute("hitTime").as_int();
	deathLimit = config.child("death").attribute("height").as_int();

	//All diferent enemy spawns
	initialPos1.x = config.child("initialPos1").attribute("x").as_int();
	initialPos1.y = config.child("initialPos1").attribute("y").as_int();
	initialPos2.x = config.child("initialPos2").attribute("x").as_int();
	initialPos2.y = config.child("initialPos2").attribute("y").as_int();
	initialPos3.x = config.child("initialPos3").attribute("x").as_int();
	initialPos3.y = config.child("initialPos3").attribute("y").as_int();
	return ret;
}

bool j1AirEnemy::Start()
{
	bool ret = true;
	//Loading assets and propierties from config file
	graphics = App->tex->Load(texPath.GetString());
	hitFx = App->audio->LoadFx(hitPath.GetString());
	flyFx = App->audio->LoadFx(flyPath.GetString());
	attackFx = App->audio->LoadFx(attackPath.GetString());
	LOG("Creating air enemy colliders");
	r_collider = { position.x, position.y + 2, 15, 8 };
	colAirEnemy = App->collision->AddCollider(r_collider, COLLIDER_ENEMY);
	if (graphics == nullptr || hitFx == -1 || flyFx == -1 || attackFx == -1 || colAirEnemy == nullptr) ret = false;
	return ret;
}

// Unload assets
bool j1AirEnemy::CleanUp()
{
	LOG("Unloading air enemy");
	if(colAirEnemy != nullptr) colAirEnemy->to_delete = true;
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1AirEnemy::Update(float dt) {

	if (life <= 0) {
		if (!dead) status = AIRENEMY_DEATH;
		else vel = { 0,0 };
	}
	else {
		if (isHit == true) {
			status = AIRENEMY_HIT;
		}
		else
		{
			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN) life = 0;
			else if (position.DistanceTo(App->scene->player->position) < 200 && App->scene->player->godmode == false) status = AIRENEMY_FLY;
			else status = AIRENEMY_IDLE;
		}
	}

	switch (status)
	{
	case AIRENEMY_IDLE:
		fly.Reset();
		current_animation = &idle;
		vel = { 0, 0 };
		break;
	case AIRENEMY_FLY:
		current_animation = &fly;
		if (App->scene->player->life > 0) {
			static iPoint origin;
			iPoint p = App->scene->player->position;	//the destination is the position of the player
			p = App->map->WorldToMap(p.x + 16, p.y + 16);
			origin = App->map->WorldToMap(position.x + 14, position.y);	//The origin is the current enemy position
			App->pathfinding->CreatePath(origin, p);//Creating path

			const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

			if (path->At(1) != NULL)
			{
				// Move enemy to player
				if (path->At(1)->x < origin.x) {
					position.x -= speed;
					flip = false;
				}

				if (path->At(1)->x > origin.x) {
					position.x += speed;
					flip = true;
				}

				if (path->At(1)->y > origin.y) {
					position.y += speed;
				}

				if (path->At(1)->y < origin.y) {
					position.y -= speed;
				}
			}
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint nextPoint = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				if (App->collision->debug)
				{
					App->render->Blit(App->scene->path_tex, nextPoint.x, nextPoint.y);
				}
			}
		}
		
		break;
	case AIRENEMY_HIT:
		attackEnable = false;
		if (hit_delay <= 0) {
			if (hit_timer == 0) {
				life -= 50;
				if (life > 0) {
					vel.x = (position.x - App->scene->player->position.x);
					hit_timer = 1;
					// Sound
					App->audio->PlayFx(hitFx, 0);
				}
				else {
					// Sound
					App->audio->PlayFx(deathFx, 0);
					//vel.x = (position.x - App->scene->player->position.x);
					status = AIRENEMY_DEATH;
				}
			}
		}
		else hit_delay -= 0.1f;

		break;
	case AIRENEMY_DEATH:
		current_animation = &death;
		if (OnGround()) {
			if (colAirEnemy != nullptr) colAirEnemy->to_delete = true;
			dead = true;
			vel.y = 0;
		}
		else {
			vel.y += gravity;
		}
		attackEnable = false;
		break;
	default:
		break;
	}

	if (hit_timer > 0) {
		hit_timer += 1;
		current_animation = &fly;

		vel.x = vel.x / 1.5f;

		if (hit_timer > hitTime)
		{
			status = AIRENEMY_IDLE;
			isHit = false;
			hit_delay = 1;
			hit_timer = 0;
		}
	}

	//Check if enemy hits player and hit it
	if (colAirEnemy->CheckCollision(App->scene->player->r_collider)) {
		if (attackEnable) {
			App->scene->player->isHit = true;
			attackEnable = false;
		}
	}
	else attackEnable = true;

	//Change position from velocity
	position.x += vel.x;
	position.y += vel.y;

	//LOG("%i %i %f %f", position.x, position.y, vel.x, vel.y);

	//Collider position
	colAirEnemy->SetPos(position.x, position.y + 2);

	Draw(dt);
	return true;
}

void j1AirEnemy::Draw(float dt)
{
	if (graphics != nullptr) {
		r = current_animation->GetCurrentFrame(dt);
		if (!flip) App->render->Blit(graphics, position.x, position.y, &r, 1.0f, 0.0f, INT_MAX, INT_MAX, SDL_FLIP_HORIZONTAL);
		else App->render->Blit(graphics, position.x, position.y, &r);

	}

	r.x = position.x;
	r.y = position.y;
}

bool j1AirEnemy::OnGround() {

	bool ret = false;
	for (int i = 0; i < App->map->groundCol.count(); i++) {
		ret = colAirEnemy->CheckCollision(App->map->groundCol.At(i)->data->rect);
		if (ret) {
			if (vel.y > 0) {
				position.y = App->map->groundCol.At(i)->data->rect.y - 15;
				vel.x = 0;
			}
			else if (vel.y < 0) {
				position.y = App->map->groundCol.At(i)->data->rect.y + 1;
				vel.x = 0;
			}
			return ret;
		}
	}
	return ret;
}

bool j1AirEnemy::Save(pugi::xml_node& data) const {
	pugi::xml_node emy = data.append_child("airenemy");

	emy.append_attribute("x") = position.x;
	emy.append_attribute("y") = position.y;

	return true;
}

bool j1AirEnemy::Load(pugi::xml_node& data)
{
	position.x = data.child("airenemy").attribute("x").as_int();
	position.y = data.child("airenemy").attribute("y").as_int();

	return true;
}