#include "EntityManager.h"

EntityManager::EntityManager()
{
	name.create("entity_manager");
	player = (j1Player*)CreateEntity(Types::player);
	airEnemy = (j1AirEnemy*)CreateEntity(Types::enemy_air);
}

//Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& a)
{
	for (unsigned int i = 0; i < entities.count(); i++)
	{
		entities.At(i)->data->Awake(a.child(entities.At(i)->data->name.GetString()));
	}
	return true;
}

// Called before the first frame
bool EntityManager::Start()
{
	for (unsigned int i = 0; i < entities.count(); i++)
	{
		entities.At(i)->data->Start();
	}
	return true;
}

// Called each loop iteration
bool EntityManager::PreUpdate()
{
	for (unsigned int i = 0; i < entities.count(); i++)
	{
		entities.At(i)->data->PreUpdate();
	}
	return true;
}

// Called each loop iteration
bool EntityManager::Update(float dt)
{
	for (unsigned int i = 0; i < entities.count(); i++)
	{
		entities.At(i)->data->Update(dt);
	}
	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	for (int i = entities.count() - 1; i >= 0; i--)
	{
		entities.At(i)->data->CleanUp();
		entities.del(entities.At(i));
	}
	entities.clear();
	return true;
}

//Called when loading the game
bool EntityManager::Load(pugi::xml_node& n)
{
	for (unsigned int i = 0; i < entities.count(); i++)
	{
		entities.At(i)->data->Load(n.child(entities.At(i)->data->name.GetString()));
	}
	return true;
}

//Called when saving the game
bool EntityManager::Save(pugi::xml_node& s) const
{
	for (unsigned int i = 0; i < entities.count(); i++)
	{
		entities.At(i)->data->Save(s.append_child(entities.At(i)->data->name.GetString()));
	}
	return true;
}

//Called when creating a new Entity
Entity* EntityManager::CreateEntity(Types type)
{
	static_assert(Types::unknown == (Types)4, "Types need update");
	Entity* ret = nullptr;
	switch (type)
	{
	case Types::player:
		//ret = new j1Player(Types::player);
		break;

	case Types::enemy_air:
		ret = new j1AirEnemy();
		break;
	case Types::enemy_ground:
		break;
	}

	if (ret != nullptr)
	{
		entities.add(ret);
		//ret->Awake();
		//ret->Start();
	}
	return ret;
}

//Called when deleting a new Entity
bool EntityManager::DeleteEntity(Entity* e)
{
	int n = entities.find(e);
	if (n == -1)return false;
	else
	{
		entities.At(n)->data->CleanUp();
		entities.del(entities.At(n));
		return true;
	}
}