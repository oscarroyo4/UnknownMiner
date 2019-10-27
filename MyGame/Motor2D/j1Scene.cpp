#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	bool ret = true;
	LOG("Loading Scene");

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{

	App->map->Load("TiledMap_2.tmx");
	App->audio->PlayMusic("audio/music/cave_ambient.ogg", 1.0f);
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
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame();

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame();

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

bool j1Scene::ChargeFirstLevel()
{
	App->player->input = false;
	App->player->CleanUp();

	p2List_item<Collider*>* item;
	for (item = App->map->groundCol.start; item != NULL; item = item->next)
		item->data->to_delete = true;
	App->map->groundCol.clear();
	App->collision->CleanUp();

	App->map->CleanUp();
	App->map->Load("TiledMap_2.tmx");
	App->audio->Awake(App->config);
	App->audio->PlayMusic("audio/music/cave_ambient.ogg", 1.0f);
	App->player->Start();
	App->player->position.x = App->player->initialX;
	App->player->position.y = App->player->initialY;
	App->player->input = true;

	level_Loaded = 1;

	return true;
}

bool j1Scene::ChargeSecondLevel()
{
	App->player->input = false;
	App->player->CleanUp();

	p2List_item<Collider*>* item;
	for (item = App->map->groundCol.start; item != NULL; item = item->next)
		item->data->to_delete = true;
	App->map->groundCol.clear();
	App->collision->CleanUp();

	App->map->CleanUp();
	App->map->Load("TiledMap.tmx");
	App->audio->Awake(App->config);
	App->audio->PlayMusic("audio/music/cave_ambient.ogg", 1.0f);
	App->player->Start();
	App->player->position.x = App->player->initialX2;
	App->player->position.y = App->player->initialY2;
	App->player->input = true;

	level_Loaded = 2;

	return true;
}
