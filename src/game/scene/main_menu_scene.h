#pragma once

struct Game;

#include "game/resource_id.h"
#include "game/ui/user_interface.h"

class MainMenuScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	struct Images {
		ImageID mario64_skybox;
		ImageID final_fantasy_menu_border;
		ImageID focus_indicator;
	} m_images;
	struct Sounds {
		SoundID menu_navigate;
	} m_sounds;
	ui::UserInterface m_ui;
	int m_menu_index = 0;
};
