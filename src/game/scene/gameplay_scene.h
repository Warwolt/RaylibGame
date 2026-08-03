#pragma once

#include "game/resource_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>

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
	ImageID m_level_background;
	Vector2 m_player_position = { 0, 0 }; // relative center of player

	void _update_pause_menu(Game* game);
	void _update_gameplay(Game* game);
};
