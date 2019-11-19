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
	idle.PushBack({ 0, 0, 32, 32 });
	idle.speed = 1.0f;

	fly.PushBack({ 32, 0, 32, 32 });
	fly.PushBack({ 96, 0, 32, 32 });
	fly.PushBack({ 64, 0, 32, 32 });
	fly.PushBack({ 96, 0, 32, 32 });
	fly.speed = 0.4f;

	death.PushBack({ 0, 96, 32, 32 });
	death.PushBack({ 32, 102, 32, 26 });
	death.PushBack({ 64, 102, 32, 26 });
	death.PushBack({ 96, 102, 32, 26 });
	death.speed = 0.1f;
	death.loop = false;

	attack.PushBack({ 0, 32, 32, 32 });
	attack.PushBack({ 32, 32, 32, 32 });
	attack.PushBack({ 64, 32, 32, 32 });
	attack.PushBack({ 96, 32, 32, 32 });
	attack.speed = 0.2f;
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
	flyFx = App->audio->LoadFx(dlyPath.GetString());
	attackFx = App->audio->LoadFx(attackPath.GetString());
	LOG("Creating player colliders");
	colEnemy = App->collision->AddCollider({ position.x, position.y, 10, 10 }, COLLIDER_ENEMY);
	return ret;
}