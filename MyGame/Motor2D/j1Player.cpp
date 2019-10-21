#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "Animation.h"
#include "j1Player.h"

//#define PUNCH_TIME 80
#define PUNCH_TIME 19

j1Player::j1Player() : j1Module()
{
	position.x = 180;
	position.y = 336;

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


	punch.PushBack({ 0, 32, 32, 32 });
	punch.PushBack({ 32, 32, 32, 32 });
	punch.PushBack({ 64, 32, 32, 32 });
	punch.PushBack({ 96, 32, 32, 32 });
	punch.speed = 0.2f;

	jump.PushBack({ 96, 0, 32, 32 });
	jump.speed = 1.0f;
}

// Destructor
j1Player::~j1Player()
{}

bool j1Player::Start()
{
	LOG("Loading player");
	graphics = App->tex->Load("maps/Character.png");
	colPlayer = App->collision->AddCollider({position.x + 9, position.y + 16, 10, 16}, COLLIDER_PLAYER);
	life = 100;
	speed = 1;
	return true;
}

// Unload assets
bool j1Player::CleanUp()
{
	LOG("Unloading player");
	colPlayer->to_delete = true;
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1Player::Update(float dt) {
	//Input
	if (input) {
		if (colPlayer->CheckCollision(App->scene->testCol->rect)) {
			
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
				status = PLAYER_BACKWARD;

			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
				status = PLAYER_FORWARD;

			else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
				status = PLAYER_PUNCH;

			else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				status = PLAYER_JUMP;

			else status = PLAYER_IDLE;
		}
		else status = PLAYER_IN_JUMP_FINISH;
	}
	//Status
	switch (status)
	{
	case PLAYER_IDLE:
		jump.Reset();
		current_animation = &idle;
		break;
	case PLAYER_FORWARD:
		position.x += speed;
		current_animation = &forward;
		break;
	case PLAYER_BACKWARD:
		position.x -= speed;
		current_animation = &backward;
		break;
	case PLAYER_JUMP:
		if (jumpEnable == true) {
			jumpEnable = false;
			jump.Reset();
			/* Sound
			if (App->sounds->Play_chunk(jumpfx))
			{
				LOG("Could not play select sound. Mix_PlayChannel: %s", Mix_GetError());
			}
			*/
			jump_timer = 1;
		}
		break;
	case PLAYER_IN_JUMP_FINISH:
		current_animation = &jump;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) { position.x--; }
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) { position.x++; }
		//Velcidad en el aire
		position.y++;

		break;
	case PLAYER_PUNCH:
		if (punchEnable == true) {
			punchEnable = false;
			punch.Reset();
			punch_timer = 1;
			/* Sound
			if (App->sounds->Play_chunk(punchfx))
			{
				LOG("Could not play select sound. Mix_PlayChannel: %s", Mix_GetError());
			}
			*/
			// Collider
			punchCol = App->collision->AddCollider({ position.x + 19, position.y + 14, 10, 18 }, COLLIDER_PLAYER_SHOT);
			//punchHit = false;
		}
		break;

	case PLAYER_IN_PUNCH_FINISH:
		status = PLAYER_IDLE;
		punch.Reset();
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

	if (jump_timer > 0)
	{
		jump_timer++;
		current_animation = &jump;
		if (jump_timer < 30) { position.y -= 2; }
		else if (jump_timer < 38) { position.y -= 1; }

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) { position.x--; }
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) { position.x++; }

		if (jump_timer > 38)
		{
			jumpEnable = true;
			status = PLAYER_IN_JUMP_FINISH;
			jump_timer = 0;
		}
	}

	//Collider position
	colPlayer->SetPos(position.x + 9, position.y + 16);

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