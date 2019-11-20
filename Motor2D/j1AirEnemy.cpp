#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1FadeToBlack.h"
#include "j1Map.h"
#include "j1Input.h"
#include "Animation.h"
#include "j1AirEnemy.h"

j1AirEnemy::j1AirEnemy() : j1Module()
{
	name.create("airenemy");

	// idle animation (arcade sprite sheet)
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
	LOG("Loading player from config_file");

	texPath = config.child("path").attribute("tex").as_string();
	hitPath = config.child("path").attribute("hit").as_string();
	flyPath = config.child("path").attribute("jump").as_string();
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

bool j1AirEnemy::Start()
{
	bool ret = true;
	//Loading assets and propierties from config file
	position.x = initialX;
	position.y = initialY;
	graphics = App->tex->Load(texPath.GetString());
	hitFx = App->audio->LoadFx(hitPath.GetString());
	flyFx = App->audio->LoadFx(flyPath.GetString());
	attackFx = App->audio->LoadFx(attackPath.GetString());
	LOG("Creating player colliders");
	colAirEnemy = App->collision->AddCollider({ position.x, position.y+2, 15, 8 }, COLLIDER_ENEMY);
	return ret;
}

// Unload assets
bool j1AirEnemy::CleanUp()
{
	LOG("Unloading player");
	colAirEnemy->to_delete = true;
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1AirEnemy::Update(float dt) {

	if (!OnGround()) {
		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN) status = AIRENEMY_DEATH;
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) status = AIRENEMY_FLY;
	}
	else {
		vel.y = 0;
	}

	switch (status)
	{
	case AIRENEMY_IDLE:
		break;
	case AIRENEMY_FLY:
		current_animation = &fly;
		vel.x = speed;
		break;
	case AIRENEMY_ATTACK:
		break;
	case AIRENEMY_ATTACK_FINISH:
		break;
	case AIRENEMY_DEATH:
		current_animation = &death;
		vel.y += gravity;
		break;
	default:
		break;
	}

	//Change position from velocity
	position.x += vel.x;
	position.y += vel.y;

	//Collider position
	if (vel.y > 0) colAirEnemy->SetPos(position.x, position.y+2);
	else colAirEnemy->SetPos(position.x, position.y + 2);
	if (vel.x > 0) 	colAirEnemy->SetPos(position.x, position.y + 2);
	else if (vel.x < 0) 	colAirEnemy->SetPos(position.x, position.y + 2);
	else colAirEnemy->SetPos(position.x, position.y + 2);

	Draw();
	return true;
}

void j1AirEnemy::Draw()
{
	if (graphics != nullptr) {
		r = current_animation->GetCurrentFrame();
		App->render->Blit(graphics, position.x, position.y, &r);

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

bool j1AirEnemy::Save(pugi::xml_node& data) const {
	pugi::xml_node emy = data.append_child("airenemy");

	emy.append_attribute("x") = position.x;
	emy.append_attribute("y") = position.y;

	return true;
}