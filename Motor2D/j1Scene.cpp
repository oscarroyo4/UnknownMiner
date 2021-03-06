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
	ambient_audio_2 = config.child("audio2").attribute("src").as_string();

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
	coin1 = App->entitymanager->CreateEntity(Types::coin);
	coin1->position = coin1->initialPos1;
	coin2 = App->entitymanager->CreateEntity(Types::coin);
	coin2->position = coin1->initialPos2;
	coin3 = App->entitymanager->CreateEntity(Types::coin);
	coin3->position = coin1->initialPos3;
	
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

	App->audio->PlayMusic(ambient_audio_2.GetString(), 1.0f);
	level_Loaded = 1;

	container = new SDL_Rect{ 0,0,3000,1000 };
	coins = 0;

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	//Coin detection for deleting them
	bool collectedNow = false;
	if (coin1->collected) 
		App->entitymanager->DeleteEntity(coin1); collectedNow = true;
	if (coin2->collected) 
		App->entitymanager->DeleteEntity(coin2); collectedNow = true;
	if (coin3->collected) 
		App->entitymanager->DeleteEntity(coin3); collectedNow = true;

	//Updating ui coins
	if (!menu && !pause_menu && collectedNow) {
		for (int i = 0; i <= coins; i++) {
			if(coinImage[i] != nullptr)
				App->gui->DeleteUIElement(coinImage[i]);
			if (i != 0) {
				coinImage[i-1] = App->gui->CreateUIElement(Type::IMAGE, nullptr, { coinOffset * i, 15, 8, 8 }, { 161, 119, 8, 8 });
			}
		}
	}

	//Updating ui player life
	if (!menu && !pause_menu) {
		float lifePercent = (float)player->life / 100;
		lifeUI->quad.w = lifePercent * 31;
		lifeUI->quad.x = 6;
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//Inputs for debug

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		App->fadetoblack->FadeToBlack(1);
		ChargeFirstLevel();
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		App->fadetoblack->FadeToBlack(2);
		ChargeSecondLevel();		
	}		

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		if (level_Loaded == 1)
		{
			App->fadetoblack->FadeToBlack(1);
			ChargeFirstLevel();
		}
		else if (level_Loaded == 2)
		{
			App->fadetoblack->FadeToBlack(2);
			ChargeSecondLevel();			
		}
	}
		
		//App->fadetoblack->FadeToBlack(level_Loaded);

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("saves/save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) 
		App->LoadGame("saves/save_game.xml");
	
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		App->capped = !App->capped;

	//Render Map
	App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	//Input for menu
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		if (!menu && !pause_menu) { CreatePauseMenu(); player->input = false; menu = false; pause_menu = true; App->audio->PlayFx(App->gui->click_sfx);}
	//Input for quit
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
	if (player != nullptr)			App->entitymanager->DeleteEntity(player);
	if (air_enemy != nullptr)		App->entitymanager->DeleteEntity(air_enemy);
	if (air_enemy2 != nullptr)		App->entitymanager->DeleteEntity(air_enemy2);
	if (ground_enemy != nullptr)	App->entitymanager->DeleteEntity(ground_enemy);
	if (ground_enemy2 != nullptr)	App->entitymanager->DeleteEntity(ground_enemy2);
	if (coin1 != nullptr)			App->entitymanager->DeleteEntity(coin1);
	if (coin2 != nullptr)			App->entitymanager->DeleteEntity(coin2);
	if (coin3 != nullptr)			App->entitymanager->DeleteEntity(coin3);


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
	coin1 = App->entitymanager->CreateEntity(Types::coin);
	coin1->position = coin1->initialPos1;
	coin2 = App->entitymanager->CreateEntity(Types::coin);
	coin2->position = coin1->initialPos2;
	coin3 = App->entitymanager->CreateEntity(Types::coin);
	coin3->position = coin1->initialPos3;

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
	if (coin1 != nullptr)			App->entitymanager->DeleteEntity(coin1);
	if (coin2 != nullptr)			App->entitymanager->DeleteEntity(coin2);
	if (coin3 != nullptr)			App->entitymanager->DeleteEntity(coin3);

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
	coin1 = App->entitymanager->CreateEntity(Types::coin);
	coin1->position = coin1->initialPos4;
	coin2 = App->entitymanager->CreateEntity(Types::coin);
	coin2->position = coin1->initialPos5;
	coin3 = App->entitymanager->CreateEntity(Types::coin);
	coin3->position = coin1->initialPos6;

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
	//player->input = false;
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
			pause_menu = false;
			CreateInGameMenu();
			App->audio->PlayMusic(ambient_audio.GetString(), 1.0f);
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
			CreateInGameMenu();
			LOG("resume");
		}
		else if (button->name == "SAVE") {
			App->SaveGame("saves/save_game.xml");
			LOG("save");
		}
		else if (button->name == "LOAD") {
			App->LoadGame("saves/save_game.xml");
			App->gui->ClearUI();			
			LOG("load");
		}
		else if (button->name == "MENU") {
			App->gui->ClearUI();
			CreateUI();	
			App->audio->PlayMusic(ambient_audio_2.GetString(), 1.0f);
			LOG("menu");
		}
		else if (button->name == "FULLSCREEN") {
			if (fullscreen)
			{
				fullscreen = false;
				LOG("exit fullscreen");
				App->gui->ClearUI();
				CreateOptions();
				SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_RESIZABLE);
			}
			else
			{
				fullscreen = true;
				LOG("fullscreen");
				App->gui->ClearUI();
				CreateOptions();
				SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN);
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
		audioText = App->gui->CreateUIElement(Type::IMAGE, window, { window_pos_x + 13, window_pos_y + 20, 25, 5 }, { 225, 183, 25, 5 });
		audioSlider = App->gui->CreateUIElement(Type::SLIDER, window, { window_pos_x + 13, window_pos_y + 30, 100, 6 }, { 222, 196, 100, 6 }, "VOLUME", { 253, 181, 15, 10 });
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
		audioText = App->gui->CreateUIElement(Type::IMAGE, window, { window_pos_x + 13, window_pos_y + 20, 25, 5 }, { 225, 183, 25, 5 });
		audioSlider = App->gui->CreateUIElement(Type::SLIDER, window, { window_pos_x + 13, window_pos_y + 30, 100, 6 }, { 222, 196, 100, 6 }, "VOLUME", { 253, 181, 15, 10 });
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
	resumeButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 9, window_pos_y + 8, 40, 12 }, { 161, 100, 32, 9 }, "RESUME", { 161, 100, 32, 9 }, { 161, 100, 32, 9 }, false, { 161, 100, 32, 9 }, this);
	saveButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 25, 32, 9 }, { 161, 137, 32, 9 }, "SAVE", { 161, 137, 32, 9 }, { 161, 137, 32, 9 }, false, { 0,0,0,0 }, this);
	loadButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 38, 32, 9 }, { 161, 146, 32, 9 }, "LOAD", { 161, 146, 32, 9 }, { 161, 146, 32, 9 }, false, { 0,0,0,0 }, this);
	optionsButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 51, 32, 9 }, { 0, 9, 32, 9 }, "OPTIONS", { 0, 9, 32, 9 }, { 0, 9, 32, 9 }, false, { 0,0,0,0 }, this);
	menuButton = App->gui->CreateUIElement(Type::BUTTON, window, { window_pos_x + 13, window_pos_y + 64, 32, 9 }, { 161, 109, 32, 9 }, "MENU", { 161, 109, 32, 9 }, { 161, 109, 32, 9 }, false, { 0,0,0,0 }, this);

	return true;
}

bool j1Scene::CreateInGameMenu() {

	App->gui->ClearUI();
	lifeBar = App->gui->CreateUIElement(Type::IMAGE, nullptr, { 5, 5, 32, 9 }, { 0, 36, 32, 9 });
	lifeUI = App->gui->CreateUIElement(Type::IMAGE, nullptr, { 5, 6, 32, 7 }, { 161, 128, 30, 7 });
	return true;
}