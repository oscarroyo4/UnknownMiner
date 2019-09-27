#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

// TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
// ----------------------------------------------------
struct TileSet {
	int firstgrid = 0;
	p2SString* name = nullptr;
	uint tilewidth = 0;
	uint tileheight = 0;
	int spacing = 0;
	int margin = 0;
	p2SString image_source = "tmw_desert_spacing.png"; 
	uint width = 0;
	uint height = 0;
};

// TODO 1: Create a struct needed to hold the information to Map node
struct MapInfo {
	enum class orientation {
		UNDEFINED = -1,
		ORTHOGONAL,
		ISOMETRIC,
		STAGGERED,
		HEXAGONAL
	};
	enum class renderorder {
		UNDEFINED = -1,
		RIGHT_DOWN,
		RIGHT_UP,
		LEFT_DOWN,
		LEFT_UP
	};

	int mapVersion = 0;
	uint width = 0;
	uint height = 0;
	uint tilewidth = 0;
	uint tileheight = 0;
	int nextobjectid = 0;
	orientation orient;
	renderorder rendord;
};


// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

private:

	bool LoadMapData(pugi::xml_node);

public:

	// TODO 1: Add your struct for map info as public for now
	MapInfo* map_info;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__