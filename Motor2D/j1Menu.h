#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
#include "Entity.h"


struct SDL_Texture;

class j1Menu : public j1Module
{
public:

	j1Menu();

	// Destructor
	virtual ~j1Menu();

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

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Change levels functions
	bool ChargeSecondLevel();
	bool ChargeFirstLevel();

	int level_Loaded;
	bool loaded;

	SDL_Texture* path_tex = nullptr;

private:

	p2SString tex1;
	p2SString tex2;
	p2SString tex3;
	p2SString music_audio;

};

#endif // __j1MENU_H__