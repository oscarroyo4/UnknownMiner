#include "Entity.h"



Entity::Entity(Types type) : entity_type(type) {

}

Entity::~Entity() {

}

bool Entity::Awake(pugi::xml_node& config) {

}

bool Entity::Start() {

}

bool Entity::PreUpdate() {

}

bool Entity::Update(float dt) {

}

bool Entity::CleanUp() {

}

bool Entity::Load(pugi::xml_node&)
{

}

//Called when saving the game
bool Entity::Save(pugi::xml_node&) const
{

}
