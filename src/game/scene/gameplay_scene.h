#pragma once

#include "game/ui/user_interface.h"

struct Game;

class GameplayScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	bool m_game_paused = false;
	ui::UserInterface m_ui;
};
