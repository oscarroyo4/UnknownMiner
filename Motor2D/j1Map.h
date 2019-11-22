#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Collision.h"

// TODO 1: Create a struct for the map layer
// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int GetP(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

struct Layer {
	p2SString	name;
	int			width;
	int			height;
	uint* data;
	Properties	properties;

	Layer() : data(NULL)
	{}

	~Layer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y * width) + x];
	}
};
	// TODO 6: Short function to get the value of x,y
	


// ----------------------------------------------------
struct TileSet
{
	// TODO 7: Create a method that receives a tile id and returns it's Rect
	SDL_Rect TileToRect(uint tileid);
	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;

};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	// TODO 2: Add a list/array of layers to the map!
	p2List<Layer*> layers;
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

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	// TODO 8: Create a method that translates x,y coordinates from map positions to world positions
	iPoint PosConverter(int x, int y);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	// TODO 3: Create a method that loads a single laye
	bool LoadLayer(pugi::xml_node& node, Layer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;
	p2List<Collider*> groundCol;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;

};

#endif // __j1MAP_H__