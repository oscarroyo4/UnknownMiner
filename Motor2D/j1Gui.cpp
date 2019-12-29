#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1Audio.h"
#include "j1Scene.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("source").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());
	click_sfx = App->audio->LoadFx("audio/fx/Click.wav");

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool mouse = false;
	int count = 0;
	if (App->input->GetMouseButtonDown(1) == KEY_DOWN || App->input->GetMouseButtonDown(1) == KEY_REPEAT) {
		for (int i = UIs.count() - 1; i >= 0 && mouse == false; i--) {
			mouse = UIs.At(i)->data->CheckMouse();
			if (mouse == true)count = i;
		}
	}
	if (mouse == true) {
		UIs.At(count)->data->Move();
	}
	for (int i = 0; i < UIs.count(); i++) {
		UIs.At(i)->data->PreUpdate();
	}
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	for (int i = 0; i < UIs.count(); i++) {
		UIs.At(i)->data->PostUpdate();
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	for (int i = UIs.count() - 1; i >= 0; i--)
	{
		UIs.At(i)->data->CleanUp();
		UIs.del(UIs.At(i));
	}
	UIs.clear();
	if (atlas) 
	{
		App->tex->UnLoad(atlas);
	}

	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

UI* j1Gui::CreateUIElement(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, p2SString str, SDL_Rect sprite2, SDL_Rect sprite3, bool drageable, SDL_Rect drag_area, j1Module* s_listener)
{
	UI* ui = nullptr;
	switch (type)
	{
	case Type::BUTTON:
		ui = new ButtonUI(Type::BUTTON, p, r, sprite, sprite2, sprite3, true, true, drag_area);
		break;
	case Type::IMAGE:
		ui = new ImageUI(Type::IMAGE, p, r, sprite, drageable, drageable, drag_area);
		break;
	case Type::WINDOW:
		ui = new WindowUI(Type::WINDOW, p, r, sprite, drageable, drageable, drag_area);
		break;
	case Type::TEXT:
		ui = new TextUI(Type::TEXT, p, r, str, drageable, drageable, drag_area);
		break;
	}

	
	ui->name = str;

	if (s_listener)
	{ 
		ui->listener = s_listener; 
	}
	else
	{
		ui->listener = nullptr;
	}

	return UIs.add(ui)->data;
}

bool j1Gui::DeleteUIElement(UI* ui) {
	int n = UIs.find(ui);
	if (n == -1)return false;
	else
	{
		UIs.At(n)->data->CleanUp();
		UIs.del(UIs.At(n));
		return true;
	}
}

void j1Gui::ChangeDebug() {
	for (int i = 0; i < UIs.count(); i++) {
		UIs.At(i)->data->debug = !UIs.At(i)->data->debug;
	}
}

void j1Gui::ChangeFocus() {
	bool exit = false;
	bool focus = false;
	int count = 0;
	for (int i = 0; i < UIs.count() && exit == false; i++) {
		bool focusable = UIs.At(i)->data->CheckFocusable();
		if (focusable == true) {
			count++;
			if (focus == true) {
				UIs.At(i)->data->focus = true;
				exit = true;
			}
			else {
				focus = UIs.At(i)->data->focus;
				UIs.At(i)->data->focus = false;
			}
		}
	}
	if (count > 0 && exit == false) {
		for (int i = 0; i < UIs.count() && exit == false; i++) {
			bool focusable = UIs.At(i)->data->CheckFocusable();
			if (focusable == true) {
				UIs.At(i)->data->focus = true;
				exit = true;
			}
		}
	}
}

void j1Gui::DeleteFocus() {
	for (int i = 0; i < UIs.count(); i++) {
		UIs.At(i)->data->focus = false;
	}
}

void j1Gui::ClearUI() 
{
	UIs.clear();
}

UI::UI(Type s_type, SDL_Rect r, UI* p, bool d, bool f, SDL_Rect d_area)
{
	name.create("UI");
	type = s_type;
	drageable = d;
	focusable = f;
	screen_rect = { r.x,r.y,r.w,r.h };
	parent = p;
	if (parent != nullptr) {
		local_rect = { screen_rect.x - parent->screen_rect.x, screen_rect.y - parent->screen_rect.y, screen_rect.w, screen_rect.h };
	}
	else {
		local_rect = screen_rect;
	}
	mask_rect = screen_rect;
	debug = false;
	focus = false;
	drag_area = d_area;
}

bool UI::PreUpdate() {
	UI* ui = this;

	screen_rect.x = local_rect.x;
	screen_rect.y = local_rect.y;
	while (ui->parent != nullptr) {
		screen_rect.x += ui->parent->local_rect.x;
		screen_rect.y += ui->parent->local_rect.y;
		ui = ui->parent;
	}

	uint win_x, win_y;
	App->win->GetWindowSize(win_x, win_y);
	mask_rect = screen_rect;

	if (parent != nullptr) {
		if (mask_rect.x < parent->mask_rect.x) {
			mask_rect.x += parent->mask_rect.x - mask_rect.x;
			mask_rect.w -= parent->mask_rect.x - mask_rect.x;
		}
		else if (mask_rect.x + mask_rect.w > parent->mask_rect.x + parent->mask_rect.w) {
			mask_rect.w -= mask_rect.x + mask_rect.w - parent->mask_rect.x - parent->mask_rect.w;
		}
		if (mask_rect.y < parent->mask_rect.y) {
			mask_rect.y += parent->mask_rect.y - mask_rect.y;
			mask_rect.h -= parent->mask_rect.y - mask_rect.y;
		}
		else if (mask_rect.y + mask_rect.h > parent->mask_rect.y + parent->mask_rect.h) {
			mask_rect.h -= mask_rect.y + mask_rect.h - parent->mask_rect.y - parent->mask_rect.h;
		}
	}
	else {
		if (mask_rect.x < 0) {
			mask_rect.w -= mask_rect.x;
			mask_rect.x = 0;
		}
		else if (mask_rect.x + mask_rect.w > win_x) {
			mask_rect.w -= mask_rect.x + mask_rect.w - win_x;
		}
		if (mask_rect.y < 0) {
			mask_rect.h -= mask_rect.y;
			mask_rect.y = 0;
		}
		else if (mask_rect.y + mask_rect.h > win_y) {
			mask_rect.h -= mask_rect.y + mask_rect.h - win_y;
		}
	}
	return true;
}

bool UI::PostUpdate() {
	if (debug == true) {
		App->render->DrawQuad(screen_rect, 255, 0, 0, 255, false, false);
	}
	return true;
}

SDL_Rect UI::GetScreenRect() 
{
	return screen_rect;
}
SDL_Rect UI::GetLocalRect() {
	return local_rect;
}
iPoint UI::GetScreenPos() {
	return { screen_rect.x,screen_rect.y};
}
iPoint UI::GetScreenToWorldPos() {
	return { screen_rect.x / (int)App->win->GetScale(),screen_rect.y / (int)App->win->GetScale() };
}
iPoint UI::GetLocalPos() {
	return { local_rect.x,local_rect.y };
}
void UI::SetLocalPos(iPoint pos) {
	iPoint r = { -local_rect.x + pos.x,-local_rect.y + pos.y };
	local_rect.x = pos.x;
	local_rect.y = pos.y;
	screen_rect.x += r.x;
	screen_rect.y += r.y;
}

bool UI::CheckMouse() {
	if (drageable == true) {
		int x, y;
		App->input->GetMouseScreenPosition(x, y);
		if (x >= screen_rect.x && x <= screen_rect.x + screen_rect.w && y >= screen_rect.y && y <= screen_rect.y + screen_rect.h || focus == true)
			return true;
	}
	return false;
}

bool UI::Move() {
	int x, y;
	App->input->GetMouseMotion(x, y);
	if (screen_rect.x + x >= drag_area.x && screen_rect.x + screen_rect.w + x <= drag_area.x + drag_area.w)
		local_rect.x += x;
	else if (screen_rect.y + y >= drag_area.y && screen_rect.y + screen_rect.h + y <= drag_area.y + drag_area.h)
		local_rect.y += y;
	return true;
}

SDL_Rect UI::Check_Printable_Rect(SDL_Rect sprite, iPoint& dif_sprite) {
	if (mask_rect.x > screen_rect.x) {
		dif_sprite.x = mask_rect.x - screen_rect.x;
		sprite.x += dif_sprite.x;
		sprite.w -= dif_sprite.x;
	}
	else if (mask_rect.w < screen_rect.w) {
		sprite.w -= screen_rect.w - mask_rect.w;
	}
	if (mask_rect.y > screen_rect.y) {
		dif_sprite.y = mask_rect.y - screen_rect.y;
		sprite.y += dif_sprite.y;
		sprite.h -= dif_sprite.y;
	}
	else if (mask_rect.h < screen_rect.h) {
		sprite.h -= screen_rect.h - mask_rect.h;
	}
	return sprite;
}

ImageUI::ImageUI(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area) {
	name.create("ImageUI");
	sprite1 = sprite;
	quad = r;
	SDL_Rect drag_area = GetDragArea();
	drag_position_0 = { drag_area.x, drag_area.y };
	drag_position_1 = { drag_area.w + drag_area.x - GetLocalRect().w,drag_area.h + drag_area.y - GetLocalRect().h };
}

bool ImageUI::PreUpdate() {
	int x, y;
	iPoint initial_position = GetScreenPos();
	App->input->GetMousePosition(x, y);
	if (CheckFocusable() == true && (x >= GetScreenPos().x && x <= GetScreenPos().x + GetScreenRect().w && y >= GetScreenPos().y && y <= GetScreenPos().y + GetScreenRect().h)) {
		if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
			App->gui->DeleteFocus();
			focus = true;
		}
	}
	if (focus == true && App->input->GetMouseButtonDown(1) == KEY_UP) {
		focus = false;
	}
	UI::PreUpdate();
	if (initial_position != GetScreenPos()) {
		fPoint drag_position = GetDragPositionNormalized();
		/////HERE LISTENER WITH DRAG POSITION
	}
	return true;
}

bool ImageUI::PostUpdate() {
	iPoint dif_sprite = { 0,0 };
	SDL_Rect sprite = UI::Check_Printable_Rect(sprite1, dif_sprite);
	quad.x = GetScreenPos().x + dif_sprite.x;
	quad.y = GetScreenPos().y + dif_sprite.y;

	App->render->BlitInQuad((SDL_Texture*)App->gui->GetAtlas(), sprite, quad);
	UI::PostUpdate();
	return true;
}

fPoint ImageUI::GetDragPositionNormalized() {
	fPoint position_normalized;
	position_normalized.x = GetScreenPos().x - drag_position_0.x;
	position_normalized.y = GetScreenPos().y - drag_position_0.y;
	position_normalized.x /= drag_position_1.x - drag_position_0.x;
	position_normalized.y /= drag_position_1.y - drag_position_0.y;
	return position_normalized;
}

WindowUI::WindowUI(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area) {
	name.create("WindowUI");
	sprite1 = sprite;
	quad = r;
}

bool WindowUI::PostUpdate() {
	iPoint dif_sprite = { 0,0 };
	SDL_Rect sprite = UI::Check_Printable_Rect(sprite1, dif_sprite);
	App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), GetScreenPos().x + dif_sprite.x, GetScreenPos().y + dif_sprite.y, &sprite, 0.f);
	UI::PostUpdate();
	return true;
}

TextUI::TextUI(Type type, UI* p, SDL_Rect r, p2SString str, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area) {
	name.create("TextUI");
	stri = str;
	quad = r;
}

bool TextUI::PostUpdate() {
	SDL_Rect rect = { 0,0,0,0 };
	iPoint dif_sprite = { 0,0 };

	SDL_Texture* text = App->fonts->Print(stri.GetString());

	SDL_QueryTexture(text, NULL, NULL, &rect.w, &rect.h);


	SDL_Rect sprite = UI::Check_Printable_Rect(rect, dif_sprite);
	App->render->Blit(text, GetScreenToWorldPos().x + dif_sprite.x, GetScreenToWorldPos().y + dif_sprite.y, &sprite, 0.f);
	UI::PostUpdate();

	App->tex->UnLoad(text);

	return true;
}

ButtonUI::ButtonUI(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, SDL_Rect spriten2, SDL_Rect spriten3, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area) {
	name.create("ButtonUI");
	sprite1 = sprite;
	sprite2 = spriten2;
	sprite3 = spriten3;
	over = false;
	pushed = false;
	if (p != nullptr) {
		quad.x = p->quad.x + r.x;
		quad.y = p->quad.y + r.y;
		quad.w = r.w;
		quad.h = r.h;
	}else quad = r;
}

bool ButtonUI::PostUpdate() {
	SDL_Rect sprite;
	iPoint dif_sprite = { 0,0 };
	if (pushed == true) {
		sprite = UI::Check_Printable_Rect(sprite2, dif_sprite);
	}
	else if (over == true) {
		sprite = UI::Check_Printable_Rect(sprite1, dif_sprite);
	}
	else {
		sprite = UI::Check_Printable_Rect(sprite3, dif_sprite);
	}
	//App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), GetScreenToWorldPos().x + dif_sprite.x, GetScreenToWorldPos().y + dif_sprite.y, &sprite, 0.f);

	quad.x = GetScreenPos().x + dif_sprite.x;
	quad.y = GetScreenPos().y + dif_sprite.y;
	App->render->BlitInQuad((SDL_Texture*)App->gui->GetAtlas(), sprite, quad);

	UI::PostUpdate();
	return true;
}

bool ButtonUI::PreUpdate() {
	int x, y;
	App->input->GetMouseScreenPosition(x, y);

	if ((x >= GetScreenPos().x && x <= GetScreenPos().x + GetScreenRect().w && y >= GetScreenPos().y && y <= GetScreenPos().y + GetScreenRect().h) || focus == true)
		over = true;
	else over = false;
	bool button = false;
	if (App->input->GetMouseButtonDown(1) == KEY_UP || App->input->GetKey(SDL_SCANCODE_RETURN))
		button = true;
	if (over == true && button == true)
		pushed = true;
	else pushed = false;

	if (pushed)
	{
		App->audio->PlayFx(App->gui->click_sfx);
		//Button clicked
		if (listener) 
		{
			listener->OnClick(this);			
		}
	}

	UI::PreUpdate();

	return true;
}
