#pragma once

#include "game/game_api.h"
#include "platform/lean_mean_windows.h"

struct GameLibrary {
	HMODULE handle;
	decltype(&Game_initialize) initialize;
	decltype(&Game_shutdown) shutdown;
	decltype(&Game_update) update;
	decltype(&Game_render) render;
};
