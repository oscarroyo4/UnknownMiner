#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1FadeToBlack.h"
#include "j1Map.h"
#include "j1Input.h"
#include "Animation.h"
#include "j1Player.h"

//#define PUNCH_TIME 80
#define PUNCH_TIME 19

j1Player::j1Player() : j1Module()
{
	name.create("player");

	// idle animation (arcade sprite sheet)
	idle.PushBack({ 0, 0, 32, 32 });
	idle.speed = 1.0f;
	
	forward.PushBack({ 32, 0, 32, 32 });
	forward.PushBack({ 96, 0, 32, 32 });
	forward.PushBack({ 64, 0, 32, 32 });
	forward.PushBack({ 96, 0, 32, 32 });
	forward.speed = 0.4f;

	backward.PushBack({ 32, 0, 32, 32 });
	backward.PushBack({ 96, 0, 32, 32 });
	backward.PushBack({ 64, 0, 32, 32 });
	backward.PushBack({ 96, 0, 32, 32 });
	backward.speed = 0.4f;

	death.PushBack({ 0, 96, 32, 32 });
	death.PushBack({ 32, 102, 32, 26 });
	death.PushBack({ 64, 102, 32, 26 });
	death.PushBack({ 96, 102, 32, 26 });
	death.speed = 0.1f;
	death.loop = false;

	punch.PushBack({ 0, 32, 32, 32 });
	punch.PushBack({ 32, 32, 32, 32 });
	punch.PushBack({ 64, 32, 32, 32 });
	punch.PushBack({ 96, 32, 32, 32 });
	punch.speed = 0.2f;

	punch_air.PushBack({ 0, 64, 32, 38 });
	punch_air.PushBack({ 32, 64, 32, 38 });
	punch_air.PushBack({ 64, 64, 32, 38 });
	punch_air.PushBack({ 96, 64, 32, 38 });
	punch_air.PushBack({ 96, 0, 32, 32 });
	punch_air.speed = 0.2f;
	punch_air.loop = false;
	
	jump.PushBack({ 96, 0, 32, 32 });
	jump.speed = 1.0f;
}

// Destructor
j1Player::~j1Player()
{}

bool j1Player::Awake(pugi::xml_node& config) {
	bool ret = true;
	LOG("Loading player from config_file");

	texPath = config.child("path").attribute("tex").as_string();
	swoshPath = config.child("path").attribute("swosh").as_string();
	hitPath = config.child("path").attribute("hit").as_string();
	jumpPath = config.child("path").attribute("jump").as_string();
	landPath = config.child("path").attribute("land").as_string();
	life = config.child("propierties").attribute("life").as_int();
	speed = config.child("propierties").attribute("speed").as_int();
	gravity = config.child("propierties").attribute("gravity").as_float();
	deathLimit = config.child("death").attribute("height").as_int();
	deathTimer_config = config.child("death").attribute("time").as_float();
	initialX = config.child("initialPos1").attribute("x").as_int();
	initialY = config.child("initialPos1").attribute("y").as_int();
	initialX2 = config.child("initialPos2").attribute("x").as_int();
	initialY2 = config.child("initialPos2").attribute("y").as_int();
	return ret;
}

bool j1Player::Start()
{
	bool ret = true;
	//Loading assets and propierties from config file
	position.x = initialX;
	position.y = initialY;
	graphics = App->tex->Load(texPath.GetString());
	swoshFx = App->audio->LoadFx(swoshPath.GetString());
	hitFx = App->audio->LoadFx(hitPath.GetString());
	jumpFx = App->audio->LoadFx(jumpPath.GetString());
	landFx = App->audio->LoadFx(landPath.GetString());
	LOG("Creating player colliders");
	colPlayer = App->collision->AddCollider({position.x + 9, position.y + 16, 10, 16}, COLLIDER_PLAYER);
	colPlayerWalls = App->collision->AddCollider({position.x + 8, position.y + 14, 12, 2}, COLLIDER_PLAYER);
	return ret;
}

// Unload assets
bool j1Player::CleanUp()
{
	LOG("Unloading player");
	colPlayer->to_delete = true;
	colPlayerWalls->to_delete = true;
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1Player::Disable() { //Disable function for changing scene
	colPlayer->to_delete = true;
	colPlayerWalls->to_delete = true;
	SDL_DestroyTexture(graphics);
	App->audio->CleanUp();
	return true;
}

bool j1Player::ResetStates() { //Reset all states before checking input
	vel.y = 0;
	jump.Reset();
	punch_air.Reset();
	jumpEnable = true;
	airTimer = deathTimer_config;
	punchAirEnable = true;

	return true;
}

bool j1Player::Update(float dt) {
	//If player falls, die
	if (position.y > deathLimit && status != PLAYER_DEATH) {
		death.Reset();
		deathTimer = deathTimer_config;
		status = PLAYER_DEATH;
		input = false;
	}
	if (App->scene->level_Loaded == 1) { //If player finishes level
		if (position.x > 3060) {
			App->fadetoblack->FadeToBlack(2);
		}
	}
	else {
		if (position.x > 3030) {
			App->fadetoblack->FadeToBlack(1);
		}
	}
	//Input
	if (input) {
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
			ResetStates();
			godmode = !godmode;
		}
		if (!godmode) {
			if (OnGround()) {
				ResetStates();
				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
					WallCollision(); //Detect horizontal collision
					if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
						status = PLAYER_JUMP;
					else status = PLAYER_BACKWARD;
				}

				else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
					WallCollision(); //Detect horizontal collision
					if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
						status = PLAYER_JUMP;
					else status = PLAYER_FORWARD;
				}

				else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
					status = PLAYER_PUNCH;

				else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
					status = PLAYER_JUMP;

				else status = PLAYER_IDLE;
			}
			else {
				WallCollision(); //Detect horizontal collision
				if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
					status = PLAYER_PUNCH_AIR;
				else
					status = PLAYER_IN_AIR;
			}
		}
		else {
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				position.x -= speed*2;
			}
			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				position.x += speed*3;
			}
			else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
				position.y -= speed;
			}
			else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
				position.y += speed;
			}
		}
				
	}
	//Status
	switch (status)
	{
	case PLAYER_IDLE:
		vel.x = 0;
		current_animation = &idle;
		break;
	case PLAYER_FORWARD:
		vel.x = speed;
		current_animation = &forward;
		break;
	case PLAYER_BACKWARD:
		vel.x = -speed;
		current_animation = &backward;
		break;
	case PLAYER_JUMP:
		if (jumpEnable == true) {
			jumpEnable = false;
			current_animation = &jump;
			vel.y = -3;
			jump.Reset();
			// Sound
			App->audio->PlayFx(jumpFx, 0);
			
			
		}
		break;
	case PLAYER_IN_AIR:
		vel.y += gravity;
		if (airTimer <= 0) vel.x = 0;
		else airTimer -= 0.1f;

		break;
	case PLAYER_PUNCH:
		if (punchEnable == true) {
			punchEnable = false;
			punch.Reset();
			punch_timer = 1;
			// Sound
			App->audio->PlayFx(hitFx, 0);

			// Collider
			punchCol = App->collision->AddCollider({ position.x + 19, position.y + 14, 10, 18 }, COLLIDER_PLAYER_SHOT);
			//punchHit = false;
		}
		break;

	case PLAYER_IN_PUNCH_FINISH:
		status = PLAYER_IDLE;
		punch.Reset();
		break;
	case PLAYER_PUNCH_AIR:
		if (punchAirEnable){
			punchAirEnable = false;
			current_animation = &punch_air;
			vel.y = -4;
			App->audio->PlayFx(swoshFx, 0);
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				airTimer = deathTimer_config;
				vel.x = 2;
				status = PLAYER_IN_AIR;
			}
			else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				airTimer = deathTimer_config;
				vel.x = -2;
				status = PLAYER_IN_AIR;

			}
		}

		break;

	case PLAYER_DEATH:
		//Death animation
		current_animation = &death;
		if (deathTimer <= 0) {
			input = true;
			if (App->scene->level_Loaded == 1) {
				position.x = initialX;
				position.y = initialY;
			}
			else {
				position.x = initialX2;
				position.y = initialY2;
			}
			status = PLAYER_IN_AIR;
		}
		else deathTimer -= 0.1f;
		break;

	default:
		break;
	}

	if (punch_timer > 0)
	{
		punch_timer = punch_timer + 1;
		current_animation = &punch;
		/* Collider and enemy hit
		if (punchCol->CheckCollision(App->enemy->r) && punchHit == false) {
			App->enemy->hit = true;
			punchHit = true;
		}
		*/
		if (punch_timer > PUNCH_TIME)
		{
			punchEnable = true;
			status = PLAYER_IN_PUNCH_FINISH;
			punchCol->to_delete = true;
			punch_timer = 0;
		}
	}

	//Change position from velocity
	position.x += vel.x;
	position.y += vel.y;

	//Collider position
	if(vel.y > 0) colPlayer->SetPos(position.x + 9, position.y + 18);
	else colPlayer->SetPos(position.x + 9, position.y + 16);
	if(vel.x > 0) 	colPlayerWalls->SetPos(position.x + 9, position.y + 14);
	else if(vel.x < 0) 	colPlayerWalls->SetPos(position.x + 7, position.y + 14);
	else colPlayerWalls->SetPos(position.x + 8, position.y + 14);


	//Function to draw the player
	Draw();

	return true;
}

void j1Player::Draw()
{
	if (graphics != nullptr) {
		r = current_animation->GetCurrentFrame();
		App->render->Blit(graphics, position.x, position.y, &r);

	}

	r.x = position.x;
	r.y = position.y;
}

bool j1Player::OnGround() {

	bool ret = false;
	for (int i = 0; i < App->map->groundCol.count(); i++) {
		ret = colPlayer->CheckCollision(App->map->groundCol.At(i)->data->rect);
		if (ret) {
			if (vel.y > 0) {
				App->audio->PlayFx(landFx, 0);
				position.y = App->map->groundCol.At(i)->data->rect.y - 32;
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

bool j1Player::WallCollision() {
	bool ret = false;

	for (int i = 0; i < App->map->groundCol.count(); i++) {
		ret = colPlayerWalls->CheckCollision(App->map->groundCol.At(i)->data->rect);
		if (ret) {
			if (vel.x > 0) {
				if(airTimer < 3) vel.x = 0;
				position.x = App->map->groundCol.At(i)->data->rect.x - 23;
			}
			else if (vel.x < 0) {
				if (airTimer < 3) vel.x = 0;
				position.x = App->map->groundCol.At(i)->data->rect.x + 11;
			}
			break;
		}
	}

	return ret;
}