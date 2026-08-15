#include "game/debug_overlay.h"

#include "game/game.h"

#include <raylib.h>

using namespace std::chrono_literals;

void initialize_debug_overlay(Game* game) {
	game->debug.overlay_text_animation = {
		{ "Rebuilding", 500ms },
		{ "Rebuilding.", 500ms },
		{ "Rebuilding..", 500ms },
		{ "Rebuilding...", 500ms },
	};
}

void render_debug_overlay(const Game& game) {
	switch (game.debug.reload_state.value()) {
		case HotReloadState::Rebuilding: {
			const std::string text = get_animation_value(game.debug.overlay_text_animation, game.debug.reload_state.last_changed(), Time::now());
			Raylib_DrawRectangle(0, 0, game.window.width(), game.window.height(), Color { 0, 0, 0, 127 });
			Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), text.c_str(), { 4, 0 }, 16, 0, YELLOW);
		} break;

		case HotReloadState::Failed: {
			Raylib_DrawRectangle(0, 0, game.window.width(), game.window.height(), Color { 0, 0, 0, 127 });
			Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), "Rebuild Failed!", { 4, 0 }, 16, 0, RED);
		} break;

		case HotReloadState::Idle:
		case HotReloadState::ReadyToReload:
			break;
	}
}
