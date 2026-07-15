#include "game/debug.h"

#include "game/game.h"

#include <raylib.h>

#include <cmath>

void render_debug_overlay(const Game& game) {
	const double elapsed_time = Raylib_GetTime() - game.debug.reload_state.last_changed();
	const double period = 1.5; // seconds
	const double t = fmod(elapsed_time, period) / period;
	std::string text;
	if (0.0 <= t && t < 0.33) {
		text = "Rebuilding.";
	} else if (0.33 <= t && t < 0.67) {
		text = "Rebuilding..";
	} else {
		text = "Rebuilding...";
	}

	switch (game.debug.reload_state.value()) {
		case HotReloadState::Rebuilding: {
			Raylib_DrawRectangle(0, 0, game.window.width(), game.window.height(), Color { 0, 0, 0, 127 });
			Raylib_DrawTextEx(game.resources.get_font(FontID(0)), text.c_str(), { 4, 0 }, 32, 0, YELLOW);
		} break;

		case HotReloadState::Failed: {
			Raylib_DrawRectangle(0, 0, game.window.width(), game.window.height(), Color { 0, 0, 0, 127 });
			Raylib_DrawTextEx(game.resources.get_font(FontID(0)), "Rebuild Failed!", { 4, 0 }, 32, 0, RED);
		} break;

		case HotReloadState::Idle:
		case HotReloadState::ReadyToReload:
			break;
	}
}
