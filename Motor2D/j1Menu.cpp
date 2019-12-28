#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1PathFinding.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1AirEnemy.h"
#include "EntityManager.h"
#include "j1Menu.h"
#include "j1Scene.h"

j1Menu::j1Menu() : j1Module()
{
	name.create("menu");
}

// Destructor
j1Menu::~j1Menu()
{}

// Called before render is available
bool j1Menu::Awake(pugi::xml_node& config)
{
	bool ret = true;
	LOG("Loading Menu");
	tex1 = config.child("textures").attribute("tex1").as_string();
	tex2 = config.child("textures").attribute("tex2").as_string();
	tex3 = config.child("textures").attribute("path_tex").as_string();
	music_audio = config.child("audio").attribute("src").as_string();

	return ret;
}

// Called before the first frame
bool j1Menu::Start()
{
	path_tex = App->tex->Load(tex3.GetString());

	loaded = false;

	App->audio->PlayMusic(music_audio.GetString(), 1.0f);
	level_Loaded = 1;

	return true;
}

// Called each loop iteration
bool j1Menu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Menu::Update(float dt)
{
	//Inputs for debug
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame("saves/save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("saves/save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(1);

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(2);

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(level_Loaded);

	return true;
}

// Called each loop iteration
bool j1Menu::PostUpdate()
{
	bool ret = true;
	//Input for quiting
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Menu::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool j1Menu::Load(pugi::xml_node&) {
	return true;
}
bool j1Menu::Save(pugi::xml_node&) const {
	return true;
}
