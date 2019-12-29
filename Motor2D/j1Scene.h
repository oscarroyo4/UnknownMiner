#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Entity.h"


struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void OnClick(UI* element);

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Change levels functions
	bool ChargeSecondLevel();
	bool ChargeFirstLevel();

	//Menus creation functions
	bool CreateUI();
	bool CreateOptions();
	bool CreateCredits();
	bool CreatePauseMenu();
	bool CreateInGameMenu();

	Collider* finishCol;
	int level_Loaded;
	bool loaded;


	Entity* player;
	Entity* air_enemy;
	Entity* air_enemy2;
	Entity* ground_enemy;
	Entity* ground_enemy2;
	Entity* coin1;
	Entity* coin2;
	Entity* coin3;

	SDL_Texture* path_tex = nullptr;
	SDL_Texture* map_line;
	iPoint map_line_pos;
	SDL_Rect* container;

	int coins;

private:
	int ambientFx;

	p2SString tex1;
	p2SString tex2;
	p2SString tex3;
	p2SString ambient_audio;

	iPoint tempP;
	iPoint tempE1;
	iPoint tempE2;


	//UI	
	bool menu = true;
	bool pause_menu = false;
	bool quit = false;
	bool fullscreen = false;
	UI* window = nullptr;
	UI* image = nullptr;
	UI* image2 = nullptr;
	UI* enterButton = nullptr;
	UI* optionsButton = nullptr;
	UI* creditsButton = nullptr;
	UI* quitButton = nullptr;
	UI* backButton = nullptr;
	UI* resumeButton = nullptr;
	UI* saveButton = nullptr;
	UI* loadButton = nullptr;
	UI* menuButton = nullptr;
	UI* fullscreeenButton = nullptr;
	UI* audioSlider = nullptr;
	UI* tick = nullptr;
	UI* coinImage[6];
	int coinOffset = 5;
	UI* lifeBar = nullptr;
	UI* lifeUI = nullptr;
};

#endif // __j1SCENE_H__