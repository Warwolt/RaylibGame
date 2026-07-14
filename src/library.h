#pragma once

#include "game/game.h"
#include "platform/lean_mean_windows.h"

#include <expected>
#include <string>

struct GameLibrary {
	HMODULE handle;
	decltype(&Game_initialize) initialize;
	decltype(&Game_shutdown) shutdown;
	decltype(&Game_update) update;
	decltype(&Game_render) render;
};

struct HotReloading {
	std::string executable_directory;
	std::string library_name;
	std::string library_copy_name;

	std::string library_path() const;
	std::string library_copy_path() const;
	std::expected<GameLibrary, std::string> load_library() const;
	void update(Game* game_state, GameLibrary* game_library);
};
