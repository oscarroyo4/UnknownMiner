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
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;
	LOG("Loading Scene");
	tex1 = config.child("textures").attribute("tex1").as_string();
	tex2 = config.child("textures").attribute("tex2").as_string();
	ambient_audio = config.child("audio").attribute("src").as_string();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	loaded = false;
	
	if (App->map->Load(tex1.GetString()) == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	App->audio->PlayMusic(ambient_audio.GetString(), 1.0f);
	level_Loaded = 1;

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//Inputs for debug
	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame("saves/save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("saves/save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(1);

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(2);

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(level_Loaded);

	//Render Map
	App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	//Input for quiting
	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool j1Scene::ChargeFirstLevel() //Changing to level 1
{
	App->player->input = false;
	App->player->CleanUp();
	App->airenemy->CleanUp();

	p2List_item<Collider*>* item;
	for (item = App->map->groundCol.start; item != NULL; item = item->next) //deleting all colliders
		item->data->to_delete = true;
	App->map->groundCol.clear();
	App->collision->CleanUp(); //Cleaning collider cache

	App->map->CleanUp();
	App->map->Load(tex1.GetString());
	App->audio->Awake(App->config);
	App->audio->PlayMusic(ambient_audio.GetString(), 1.0f); //Playing audio again
	App->player->Start();
	App->airenemy->Start();
	if (!loaded) {
		App->player->position.x = App->player->initialX; //Load position from config_file
		App->player->position.y = App->player->initialY;
	}
	else {
		App->player->position.x = tempX; //Load position from save_file
		App->player->position.y = tempY;
	}
	App->player->input = true;

	level_Loaded = 1;

	return true;
}

bool j1Scene::ChargeSecondLevel() //Changing to level 2
{
	App->player->input = false;
	App->player->CleanUp();
	App->airenemy->CleanUp();

	p2List_item<Collider*>* item;
	for (item = App->map->groundCol.start; item != NULL; item = item->next) //deleting all colliders
		item->data->to_delete = true;
	App->map->groundCol.clear();
	App->collision->CleanUp();  //Cleaning collider cache

	App->map->CleanUp();
	App->map->Load(tex2.GetString());
	App->audio->Awake(App->config);
	App->audio->PlayMusic(ambient_audio.GetString(), 1.0f); //Playing audio again
	App->player->Start();
	App->airenemy->Start();
	if (!loaded) {
		App->player->position.x = App->player->initialX2; //Load position from config_file
		App->player->position.y = App->player->initialY2;
	}
	else { 
		App->player->position.x = tempX; //Load position from save_file
		App->player->position.y = tempY;
	}

	App->player->input = true;

	level_Loaded = 2;

	return true;
}

bool j1Scene::Load(pugi::xml_node& data) {
	level_Loaded = data.child("scene").attribute("level").as_int();
	App->player->input = false;
	if (level_Loaded == 1) {
		App->fadetoblack->FadeToBlack(1);
		
	}
	else if (level_Loaded == 2) {
		App->fadetoblack->FadeToBlack(2);
	}
	loaded = true;
	tempX = data.parent().child("player").child("player").attribute("x").as_int();
	tempY = data.parent().child("player").child("player").attribute("y").as_int();
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const {
	pugi::xml_node ply = data.append_child("scene");

	ply.append_attribute("level") = level_Loaded;

	return true;
}
