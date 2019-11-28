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
#include <math.h>

j1GroundEnemy::j1GroundEnemy() : Entity(Types::enemy_ground)
{
	name.create("groundenemy");

	// idle animation (arcade sprite sheet)
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

	death.PushBack({ 0, 48, 18, 16 });
	death.PushBack({ 18, 48, 18, 16 });
	death.PushBack({ 36, 48, 18, 16 });
	death.PushBack({ 54, 48, 18, 16 });
	death.PushBack({ 72, 48, 18, 16 });
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
j1GroundEnemy::~j1GroundEnemy()
{}

bool j1GroundEnemy::Awake(pugi::xml_node& config) {
	bool ret = true;
	LOG("Loading ground enemy from config_file");

	texPath = config.child("path").attribute("tex").as_string();
	hitPath = config.child("path").attribute("hit").as_string();
	movePath = config.child("path").attribute("jump").as_string();
	attackPath = config.child("path").attribute("land").as_string();
	life = config.child("propierties").attribute("life").as_int();
	speed = config.child("propierties").attribute("speed").as_int();
	gravity = config.child("propierties").attribute("gravity").as_float();
	deathLimit = config.child("death").attribute("height").as_int();
	initialX = config.child("initialPos1").attribute("x").as_int();
	initialY = config.child("initialPos1").attribute("y").as_int();
	initialX2 = config.child("initialPos2").attribute("x").as_int();
	initialY2 = config.child("initialPos2").attribute("y").as_int();
	punchTime = config.child("propierties").attribute("punchTime").as_int();
	return ret;
}

bool j1GroundEnemy::Start()
{
	bool ret = true;
	//Loading assets and propierties from config file
	position.x = initialX;
	position.y = initialY;
	graphics = App->tex->Load(texPath.GetString());
	hitFx = App->audio->LoadFx(hitPath.GetString());
	moveFx = App->audio->LoadFx(movePath.GetString());
	attackFx = App->audio->LoadFx(attackPath.GetString());
	LOG("Creating ground enemy colliders");
	colGroundEnemy = App->collision->AddCollider({ position.x + 3, position.y + 7, 15, 9 }, COLLIDER_ENEMY);
	//if (graphics == nullptr || hitFx == 0 || moveFx == 0 || attackFx == 0 || collider == nullptr) ret = false;
	return ret;
}

// Unload assets
bool j1GroundEnemy::CleanUp()
{
	LOG("Unloading ground enemy");
	colGroundEnemy->to_delete = true;
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1GroundEnemy::Update(float dt) {
	if (life <= 0) {
		status = GROUNDENEMY_DEATH;
	}
	else {
		if (OnGround()) {
			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN) life = 0;
			else if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) status = GROUNDENEMY_MOVE;
			else status = GROUNDENEMY_IDLE;
		}
		else {
			status = GROUNDENEMY_IN_AIR;
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
		current_animation = &idle;
		vel.y += gravity;
		break;
	case GROUNDENEMY_MOVE:
		
		//pathSteps = App->pathfinding->CreatePath(position, App->player->position);
		current_animation = &move;
		vel.x = speed;
		/*
		for (int i = 0; i <= pathSteps; i++) {
			nextPos.x = App->pathfinding->GetLastPath()->At(i)->x;
			nextPos.y = App->pathfinding->GetLastPath()->At(i)->y;
			while (position != nextPos) {
				iPoint p = (position - nextPos).Normalize();
				vel = { (float)p.x, (float)p.y };

			}
		}
		*/
		break;
	case GROUNDENEMY_ATTACK:
		break;
	case GROUNDENEMY_ATTACK_FINISH:
		break;
	case GROUNDENEMY_DEATH:
		current_animation = &death;
		break;
	default:
		break;
	}

	//Change position from velocity
	position.x += vel.x;
	position.y += vel.y;

	//Collider position
	if (vel.y > 0) colGroundEnemy->SetPos(position.x + 3, position.y + 7);
	else colGroundEnemy->SetPos(position.x + 3, position.y + 7);
	if (vel.x > 0) 	colGroundEnemy->SetPos(position.x + 3, position.y + 7);
	else if (vel.x < 0) 	colGroundEnemy->SetPos(position.x + 3, position.y + 7);
	else colGroundEnemy->SetPos(position.x + 3, position.y + 7);

	Draw();
	return true;
}

void j1GroundEnemy::Draw()
{
	if (graphics != nullptr) {
		r = current_animation->GetCurrentFrame();
		App->render->Blit(graphics, position.x, position.y, &r);

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
				App->audio->PlayFx(attackFx, 0);
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

bool j1GroundEnemy::Save(pugi::xml_node & data) const {
	pugi::xml_node emy = data.append_child("groundenemy");

	emy.append_attribute("x") = position.x;
	emy.append_attribute("y") = position.y;

	return true;
}