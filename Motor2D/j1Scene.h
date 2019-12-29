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

	bool CreateUI();
	bool CreateOptions();
	bool CreateCredits();

	Collider* finishCol;
	int level_Loaded;
	bool loaded;


	Entity* player;
	Entity* air_enemy;
	Entity* air_enemy2;
	Entity* ground_enemy;
	Entity* ground_enemy2;
	Entity* coin;

	SDL_Texture* path_tex = nullptr;
	SDL_Texture* map_line;
	iPoint map_line_pos;
	SDL_Rect* container;

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
	bool quit = false;
	UI* window = nullptr;
	UI* image = nullptr;
	UI* image2 = nullptr;
	UI* enterButton = nullptr;
	UI* optionsButton = nullptr;
	UI* creditsButton = nullptr;
	UI* quitButton = nullptr;
	UI* backButton = nullptr;
};

#endif // __j1SCENE_H__