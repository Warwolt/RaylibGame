#pragma once

struct Game;

#include "game/resource_id.h"
#include "game/ui/user_interface.h"

#include <string_view>

class MenuUserInterface : public ui::UserInterface {
public:
	void initialize(Game* game);
	void menu_begin();
	bool menu_item(const Input& input, const ResourceManager& resources, std::string_view label);
	void menu_end();

private:
	struct Images {
		ImageID focus_indicator;
	} m_images;

	struct Sounds {
		SoundID menu_navigate;
	} m_sounds;
};

class MainMenuScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	MenuUserInterface m_ui;
};
