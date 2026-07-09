#pragma once

struct Game;

#include "game/resource_id.h"
#include "game/ui.h"

class MainMenuScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	struct Images {
		ImageID mario64_skybox;
		ImageID developer_face;
		ImageID final_fantasy_menu_border;
	} m_images;
	ui::UserInterface m_ui;
};
