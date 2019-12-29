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
#include "j1Scene.h"
#include "j1Gui.h"

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
	tex3 = config.child("textures").attribute("path_tex").as_string();
	ambient_audio = config.child("audio").attribute("src").as_string();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	path_tex = App->tex->Load(tex3.GetString());
	map_line = App->tex->Load("textures/MapLine.png");

	loaded = false;
	player = App->entitymanager->CreateEntity(Types::player);
	air_enemy = App->entitymanager->CreateEntity(Types::enemy_air);
	air_enemy->position = air_enemy->initialPos1;
	ground_enemy = App->entitymanager->CreateEntity(Types::enemy_ground);
	ground_enemy->position = ground_enemy->initialPos1;
	ground_enemy2 = App->entitymanager->CreateEntity(Types::enemy_ground);
	ground_enemy2->position = ground_enemy->initialPos2;
	coin = App->entitymanager->CreateEntity(Types::coin);

	//App->map->Load(tex1.GetString());
	
	if (App->map->Load(tex1.GetString()) == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	player->input = false;

	CreateUI();

	App->audio->PlayMusic(ambient_audio.GetString(), 1.0f);
	level_Loaded = 1;

	container = new SDL_Rect{ 0,0,3000,1000 };

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

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(1);

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(2);

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		App->fadetoblack->FadeToBlack(level_Loaded);

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("saves/save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->LoadGame("saves/save_game.xml");

	//Render Map
	App->map->Draw();

	//render preogress bar
	//map_line_pos.x = 0;
	//map_line_pos.y = 0;
	//App->render->Blit(map_line, map_line_pos.x, map_line_pos.y, container);

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	//Input for quiting
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		if (!menu && !pause_menu) { CreatePauseMenu(); player->input = false; menu = false; pause_menu = true; }
	if(App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN || quit)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->tex->UnLoad(path_tex);
	App->tex->UnLoad(map_line);

	return true;
}

bool j1Scene::ChargeFirstLevel() //Changing to level 1
{	
	player->input = false;
	if(player != nullptr)			App->entitymanager->DeleteEntity(player);
	if (air_enemy != nullptr)		App->entitymanager->DeleteEntity(air_enemy);
	if (air_enemy2 != nullptr)		App->entitymanager->DeleteEntity(air_enemy2);
	if (ground_enemy != nullptr)	App->entitymanager->DeleteEntity(ground_enemy);
	if (ground_enemy2 != nullptr)	App->entitymanager->DeleteEntity(ground_enemy2);
	if (coin != nullptr)			App->entitymanager->DeleteEntity(coin);


	p2List_item<Collider*>* item;
	for (item = App->map->groundCol.start; item != NULL; item = item->next) //deleting all colliders
		item->data->to_delete = true;
	App->map->groundCol.clear();
	App->collision->CleanUp(); //Cleaning collider cache
	App->pathfinding->CleanUp();
	App->map->CleanUp();
	if (App->map->Load(tex1.GetString()) == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	App->audio->Awake(App->config);
	App->audio->PlayMusic(ambient_audio.GetString(), 1.0f); //Playing audio again
	//Create entities
	player = App->entitymanager->CreateEntity(Types::player);
	air_enemy = App->entitymanager->CreateEntity(Types::enemy_air);
	air_enemy->position = air_enemy->initialPos1;
	ground_enemy = App->entitymanager->CreateEntity(Types::enemy_ground);
	ground_enemy->position = ground_enemy->initialPos1;
	ground_enemy2 = App->entitymanager->CreateEntity(Types::enemy_ground);
	ground_enemy2->position = ground_enemy->initialPos2;
	coin = App->entitymanager->CreateEntity(Types::coin);

	if (!loaded) {
		player->position = player->initialPos1; //Load position from config_file
	}
	else {
		player->position = tempP; //Load position from save_file
	}

	player->input = true;

	level_Loaded = 1;

	return true;
}

bool j1Scene::ChargeSecondLevel() //Changing to level 2
{
	player->input = false;
	if (player != nullptr)			App->entitymanager->DeleteEntity(player);
	if (air_enemy != nullptr)		App->entitymanager->DeleteEntity(air_enemy);
	if (air_enemy2 != nullptr)		App->entitymanager->DeleteEntity(air_enemy2);
	if (ground_enemy != nullptr)	App->entitymanager->DeleteEntity(ground_enemy);
	if (ground_enemy2 != nullptr)	App->entitymanager->DeleteEntity(ground_enemy2);
	if (coin != nullptr)			App->entitymanager->DeleteEntity(coin);

	p2List_item<Collider*>* item;
	for (item = App->map->groundCol.start; item != NULL; item = item->next) //deleting all colliders
		item->data->to_delete = true;
	App->map->groundCol.clear();
	App->collision->CleanUp();  //Cleaning collider cache
	App->pathfinding->CleanUp();
	App->map->CleanUp();
	if (App->map->Load(tex2.GetString()) == true)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	App->audio->Awake(App->config);
	App->audio->PlayMusic(ambient_audio.GetString(), 1.0f); //Playing audio again
	//Create entities
	player = App->entitymanager->CreateEntity(Types::player);
	air_enemy = App->entitymanager->CreateEntity(Types::enemy_air);
	air_enemy->position = air_enemy->initialPos2;
	air_enemy2 = App->entitymanager->CreateEntity(Types::enemy_air);
	air_enemy2->position = air_enemy->initialPos3;
	ground_enemy = App->entitymanager->CreateEntity(Types::enemy_ground);
	ground_enemy->position = ground_enemy->initialPos3;
	ground_enemy2 = App->entitymanager->CreateEntity(Types::enemy_ground);
	ground_enemy2->position = ground_enemy->initialPos4;
	coin = App->entitymanager->CreateEntity(Types::coin);

	if (!loaded) {
		player->position = player->initialPos2; //Load position from config_file
	}
	else { 
		player->position = tempP; //Load position from save_file
	}

	player->input = true;

	level_Loaded = 2;

	return true;
}

bool j1Scene::Load(pugi::xml_node& data) {
	level_Loaded = data.child("scene").attribute("level").as_int();
	player->input = false;
	if (level_Loaded == 1) {
		App->fadetoblack->FadeToBlack(1);
		
	}
	else if (level_Loaded == 2) {
		App->fadetoblack->FadeToBlack(2);
	}
	loaded = true;
	tempP.x = data.parent().child("player").child("player").attribute("x").as_int();
	tempP.y = data.parent().child("player").child("player").attribute("y").as_int();
	tempE1.x = data.parent().child("airenemy").child("airenemy").attribute("x").as_int();
	tempE1.y = data.parent().child("airenemy").child("airenemy").attribute("y").as_int();
	tempE2.x = data.parent().child("groundenemy").child("groundenemy").attribute("x").as_int();
	tempE2.y = data.parent().child("groundenemy").child("groundenemy").attribute("y").as_int();

	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const {
	pugi::xml_node ply = data.append_child("scene");

	ply.append_attribute("level") = level_Loaded;

	return true;
}

bool j1Scene::CreateUI() {

	App->gui->ClearUI();

	menu = true;

	int window_pos_x = 260;
	int window_pos_y = 10;

	image = App->gui->CreateUIElement(Type::IMAGE, nullptr, { 0, 0, 320, 180 }, { 0, 46, 160, 84 });
	image2 = App->gui->CreateUIElement(Type::IMAGE, nullptr, { 40, 50, 70, 35 }, { 84, 21, 43, 19 });
	window = App->gui->CreateUIElement(Type::WINDOW, nullptr, { window_pos_x, window_pos_y, 48, 42 }, { 34, 0, 48, 42 });
	enterButton = App->gui->CreateUIElement(Type::BUTTON, nullptr, { 140, 110, 48, 14 }, { 0, 0, 32, 9 }, "ENTER", { 0, 0, 32, 9 }, { 0, 0, 32, 9 }, false, { 0,0,0,0 }, this);
	optionsButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x +8, window_pos_y+5, 32, 9 }, { 0, 9, 32, 9 }, "OPTIONS", { 0, 9, 32, 9 }, { 0, 9, 32, 9 }, false, { 0,0,0,0 }, this);
	creditsButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 8, window_pos_y + 17, 32, 9 }, { 0, 18, 32, 9 }, "CREDITS", { 0, 18, 32, 9 }, { 0, 18, 32, 9 }, false, { 0,0,0,0 }, this);
	quitButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x +8, window_pos_y+29, 32, 9 }, { 0, 27, 32, 9 }, "QUIT", { 0, 27, 32, 9 }, { 0, 27, 32, 9 }, false, { 0,0,0,0 }, this);

	return true;
}

void j1Scene::OnClick(UI* element) {
	if (element->type == Type::BUTTON)
	{
		ButtonUI* button = (ButtonUI*)element;

		if (button->name == "ENTER") {
			App->gui->ClearUI();
			player->input = true;
			menu = false;
			LOG("enter");
		}
		else if (button->name == "OPTIONS") {
			App->gui->ClearUI();
			CreateOptions();
			LOG("options");
		}
		else if (button->name == "CREDITS") {
			App->gui->ClearUI();
			CreateCredits();
			LOG("credits");
		}
		else if (button->name == "BACK") {
			App->gui->ClearUI();
			if (menu)
			{
				CreateUI();
			}
			else
			{
				CreatePauseMenu();
			}
			
			LOG("back");
		}
		else if (button->name == "RESUME") {
			App->gui->ClearUI();
			player->input = true;
			menu = false;
			pause_menu = false;
			LOG("resume");
		}
		else if (button->name == "MENU") {
			App->gui->ClearUI();
			CreateUI();
			
			LOG("menu");
		}
		else if (button->name == "FULLSCREEN") {
			if (fullscreen)
			{
				fullscreen = false;
				LOG("exit fullscreen");
				App->gui->ClearUI();
				CreateOptions();
			}
			else
			{
				fullscreen = true;
				LOG("fullscreen");
				App->gui->ClearUI();
				CreateOptions();
			}
			
		}
		else if (button->name == "QUIT") {
			quit = true;
		}
	}
}

bool j1Scene::CreateOptions() {

	App->gui->ClearUI();

	int window_pos_x = 70;
	int window_pos_y = 40;
	if (menu)
	{
		image = App->gui->CreateUIElement(Type::IMAGE, nullptr, { 0, 0, 320, 180 }, { 0, 46, 160, 84 });
		window = App->gui->CreateUIElement(Type::WINDOW, nullptr, { window_pos_x, window_pos_y, 170, 99 }, { 160, 0, 170, 99 });
		if (fullscreen)
		{
			fullscreeenButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 53, 55, 11 }, { 163, 191, 55, 11 }, "FULLSCREEN", { 163, 191, 55, 11 }, { 163, 191, 55, 11 }, false, { 0,0,0,0 }, this);
		}
		else
		{
			fullscreeenButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 55, 53, 9 }, { 163, 181, 53, 9 }, "FULLSCREEN", { 163, 181, 53, 9 }, { 163, 181, 53, 9 }, false, { 0,0,0,0 }, this);	
		}
		backButton = App->gui->CreateUIElement(Type::BUTTON, nullptr, { 10, 10, 17, 11 }, { 137, 31, 17, 11 }, "BACK", { 137, 31, 17, 11 }, { 137, 31, 17, 11 }, false, { 0,0,0,0 }, this);
		
	}
	else
	{
		window = App->gui->CreateUIElement(Type::WINDOW, nullptr, { window_pos_x, window_pos_y, 170, 99 }, { 160, 0, 170, 99 });
		backButton = App->gui->CreateUIElement(Type::BUTTON, nullptr, { 10, 10, 17, 11 }, { 137, 31, 17, 11 }, "BACK", { 137, 31, 17, 11 }, { 137, 31, 17, 11 }, false, { 0,0,0,0 }, this);
	}

	

	return true;
}

bool j1Scene::CreateCredits() {

	App->gui->ClearUI();

	int window_pos_x = 110;
	int window_pos_y = 40;

	image = App->gui->CreateUIElement(Type::IMAGE, nullptr, { 0, 0, 320, 180 }, { 0, 131, 160, 84 });
	backButton = App->gui->CreateUIElement(Type::BUTTON, nullptr, { 10, 10, 17, 11 }, { 137, 31, 17, 11 }, "BACK", { 137, 31, 17, 11 }, { 137, 31, 17, 11 }, false, { 0,0,0,0 }, this);

	return true;
}

bool j1Scene::CreatePauseMenu() {

	App->gui->ClearUI();

	int window_pos_x = 130;
	int window_pos_y = 45;

	window = App->gui->CreateUIElement(Type::WINDOW, nullptr, { window_pos_x, window_pos_y, 57, 79 }, { 196, 101, 57, 79 });
	resumeButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 9, window_pos_y + 15, 40, 12 }, { 161, 100, 32, 9 }, "RESUME", { 161, 100, 32, 9 }, { 161, 100, 32, 9 }, false, { 161, 100, 32, 9 }, this);
	optionsButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 40, 32, 9 }, { 0, 9, 32, 9 }, "OPTIONS", { 0, 9, 32, 9 }, { 0, 9, 32, 9 }, false, { 0,0,0,0 }, this);
	menuButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 55, 32, 9 }, { 161, 109, 32, 9 }, "MENU", { 161, 109, 32, 9 }, { 161, 109, 32, 9 }, false, { 0,0,0,0 }, this);

	return true;
}