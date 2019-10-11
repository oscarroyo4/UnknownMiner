#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "Animation.h"
#include "j1Player.h"

j1Player::j1Player() : j1Module()
{
	position.x = 0;
	position.y = 0;

	// idle animation (arcade sprite sheet)
	idle.PushBack({ 0, 0, 16, 16 });
	idle.speed = 0.1f;
}

// Destructor
j1Player::~j1Player()
{}

bool j1Player::Start()
{
	LOG("Loading player");
	graphics = App->tex->Load("maps/Player_placeholder.png");
	life = 100;
	return true;
}

// Unload assets
bool j1Player::CleanUp()
{
	LOG("Unloading player");
	SDL_DestroyTexture(graphics);
	return true;
}

bool j1Player::Update() {
	//Input
	if (input) {
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			status = PLAYER_IDLE;
	}
	//Status
	switch (status)
	{
	case PLAYER_IDLE:
		current_animation = &idle;
		break;
	case PLAYER_WALK:
		break;
	case PLAYER_JUMP:
		break;
	case PLAYER_INJUMPFINISH:
		break;
	default:
		break;
	}
}