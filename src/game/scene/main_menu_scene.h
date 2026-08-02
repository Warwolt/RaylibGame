#pragma once

#include "game/ui/user_interface.h"

struct Game;

class MainMenuScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	ui::UserInterface m_ui;
};
