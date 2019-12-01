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
#include "j1GroundEnemy.h"
#include "EntityManager.h"
#include <math.h>

j1GroundEnemy::j1GroundEnemy() : Entity(Types::enemy_ground)
{
	name.create("groundenemy");

	//animations
	idle.PushBack({ 0, 0, 18, 16 });
	idle.PushBack({ 18, 0, 18, 16 });
	idle.PushBack({ 36, 0, 18, 16 });
	idle.PushBack({ 54, 0, 18, 16 });
	idle.PushBack({ 72, 0, 18, 16 });
	idle.speed = 0.2f;

	move.PushBack({ 0, 16, 18, 16 });
	move.PushBack({ 18, 16, 18, 16 });
	move.PushBack({ 36, 16, 18, 16 });
	move.PushBack({ 54, 16, 18, 16 });
	move.PushBack({ 72, 16, 18, 16 });
	move.speed = 0.3f;

	hit.PushBack({ 0, 32, 18, 16 });
	hit.PushBack({ 18, 32, 18, 16 });
	hit.PushBack({ 36, 32, 18, 16 });
	hit.PushBack({ 54, 32, 18, 16 });
	hit.PushBack({ 72, 32, 18, 16 });
	hit.speed = 0.3f;

	death.PushBack({ 0, 48, 18, 16 });
	death.PushBack({ 18, 48, 18, 16 });
	death.PushBack({ 36, 48, 18, 16 });
	death.PushBack({ 54, 48, 18, 16 });
	death.PushBack({ 72, 48, 18, 16 });
	death.speed = 0.3f;
	death.loop = false;

	inair.PushBack({ 54, 16, 18, 16 });

	/*
	attack.PushBack({ 0, 32, 32, 32 });
	attack.PushBack({ 32, 32, 32, 32 });
	attack.PushBack({ 64, 32, 32, 32 });
	attack.PushBack({ 96, 32, 32, 32 });
	attack.speed = 0.2f;
	*/
}

// Destructor
j1GroundEnemy::~j1GroundEnemy()
{}

bool j1GroundEnemy::Awake(pugi::xml_node& config) {
	bool ret = true;
	LOG("Loading ground enemy from config_file");

	texPath = config.child("path").attribute("tex").as_string();
	hitPath = config.child("path").attribute("hit").as_string();
	movePath = config.child("path").attribute("move").as_string();
	attackPath = config.child("path").attribute("attack").as_string();
	deathPath = config.child("path").attribute("death").as_string();
	life = config.child("propierties").attribute("life").as_int();
	speed = config.child("propierties").attribute("speed").as_int();
	gravity = config.child("propierties").attribute("gravity").as_float();
	hitTime = config.child("propierties").attribute("hitTime").as_int();
	deathLimit = config.child("death").attribute("height").as_int();
	initialPos1.x = config.child("initialPos1").attribute("x").as_int();
	initialPos1.y = config.child("initialPos1").attribute("y").as_int();
	initialPos2.x = config.child("initialPos2").attribute("x").as_int();
	initialPos2.y = config.child("initialPos2").attribute("y").as_int();
	initialPos3.x = config.child("initialPos3").attribute("x").as_int();
	initialPos3.y = config.child("initialPos3").attribute("y").as_int();
	initialPos4.x = config.child("initialPos4").attribute("x").as_int();
	initialPos4.y = config.child("initialPos4").attribute("y").as_int();
	return ret;
}

bool j1GroundEnemy::Start()
{
	bool ret = true;
	//Loading assets and propierties from config file
	graphics = App->tex->Load(texPath.GetString());
	hitFx = App->audio->LoadFx(hitPath.GetString());
	moveFx = App->audio->LoadFx(movePath.GetString());
	attackFx = App->audio->LoadFx(attackPath.GetString());
	deathFx = App->audio->LoadFx(deathPath.GetString());
	hit_delay = 1.0f;
	LOG("Creating ground enemy colliders");
	r_collider = { position.x + 3, position.y + 7, 15, 9 };
	colGroundEnemy = App->collision->AddCollider(r_collider, COLLIDER_ENEMY);
	//if (graphics == nullptr || hitFx == 0 || moveFx == 0 || attackFx == 0 || collider == nullptr) ret = false;
	return ret;
}

// Unload assets
bool j1GroundEnemy::CleanUp()
{
	LOG("Unloading ground enemy");
	if(colGroundEnemy != nullptr) colGroundEnemy->to_delete = true;
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1GroundEnemy::Update(float dt) {
	if (life <= 0) {
		status = GROUNDENEMY_DEATH;
	}
	else {
		if (isHit == true) {
			status = GROUNDENEMY_HIT;
		}
		else 
		{
			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN) life = 0;
			if (position.DistanceTo(App->scene->player->position) < 200 && App->scene->player->godmode == false) {
				if (OnGround()) {
					status = GROUNDENEMY_MOVE;
				}
				else {
					status = GROUNDENEMY_IN_AIR;
				}
			}
			else status = GROUNDENEMY_IDLE;

		}
	}
	
	switch (status)
	{
	case GROUNDENEMY_IDLE:
		move.Reset();
		current_animation = &idle;
		vel = {0, 0};
		break;
	case GROUNDENEMY_IN_AIR:
		current_animation = &inair;
		vel.y += gravity;
		break;
	case GROUNDENEMY_MOVE:
		current_animation = &move;
		if (App->scene->player->life > 0) {
			static iPoint origin;
			iPoint p = App->scene->player->position;	//the destination is the position of the player
			p = App->map->WorldToMap(p.x + 16, p.y + 16);
			origin = App->map->WorldToMap(position.x + 16, position.y);	//The origin is the current enemy position
			App->pathfinding->CreatePath(origin, p); //Creating path

			const p2DynArray<iPoint> * path = App->pathfinding->GetLastPath();

			if (path->At(1) != NULL)
			{
				// Move enemy to player
				if (path->At(1)->x < origin.x) {
					vel.x = -speed;
					App->audio->PlayFx(moveFx, 0);
					flip = false;
				}

				else if (path->At(1)->x > origin.x) {
					vel.x = speed;
					App->audio->PlayFx(moveFx, 0);
					flip = true;
				}
				
				if (path->At(1)->y > origin.y || path->At(1)->y < origin.y) {
					current_animation = &idle;
					vel.x = 0;
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
	case GROUNDENEMY_HIT:
		if (hit_delay <= 0) {
			if (hit_timer == 0) {
				life -= 50;
				hit.Reset();
				if (life > 0) {
					vel.x = (position.x - App->scene->player->position.x);
					hit_timer = 1;
					// Sound
					App->audio->PlayFx(hitFx, 0);
				}
				else {
					// Sound
					App->audio->PlayFx(deathFx, 0);
					attackEnable = false;
					status = GROUNDENEMY_DEATH;
				}
			}
		}
		else hit_delay -= 0.1f;

		break;
	case GROUNDENEMY_DEATH:
		current_animation = &death;
		if (colGroundEnemy != nullptr) colGroundEnemy->to_delete = true;
		attackEnable = false;
		break;
	default:
		break;
	}

	//Timer animation when player hits enemy
	if (hit_timer > 0) {
		hit_timer += 1;
		current_animation = &hit;

		vel.x = vel.x/2.0f;
		
		if (hit_timer > hitTime)
		{
			status = GROUNDENEMY_IDLE;
			isHit = false;
			hit_delay = 1;
			hit_timer = 0;
		}
	}

	//Check if enemy hits player and hit it
	if (colGroundEnemy->CheckCollision(App->scene->player->r_collider)) {
		if (attackEnable) {
			App->scene->player->isHit = true;
			attackEnable = false;
		}
	}
	else attackEnable = true;

	//Change position from velocity
	position.x += vel.x;
	position.y += vel.y;

	//Collider position

	colGroundEnemy->SetPos(position.x + 3, position.y + 7);
	   	  
	Draw(dt);
	return true;
}

void j1GroundEnemy::Draw(float dt)
{
	if (graphics != nullptr) {
		r = current_animation->GetCurrentFrame(dt);
		if (!flip) App->render->Blit(graphics, position.x, position.y, &r, 1.0f, 0.0f, INT_MAX, INT_MAX, SDL_FLIP_HORIZONTAL);
		else App->render->Blit(graphics, position.x, position.y, &r);
	}
	r.x = position.x;
	r.y = position.y;
}

bool j1GroundEnemy::OnGround() {

	bool ret = false;
	for (int i = 0; i < App->map->groundCol.count(); i++) {
		ret = colGroundEnemy->CheckCollision(App->map->groundCol.At(i)->data->rect);
		if (ret) {
			if (vel.y > 0) {
				position.y = App->map->groundCol.At(i)->data->rect.y - 16;
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

bool j1GroundEnemy::Save(pugi::xml_node & data) const {
	pugi::xml_node emy = data.append_child("groundenemy");

	emy.append_attribute("x") = position.x;
	emy.append_attribute("y") = position.y;

	return true;
}