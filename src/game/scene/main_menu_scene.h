#pragma once

struct Game;

#include "game/ui.h"

class MainMenuScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	ui::Element m_root_element;
};
