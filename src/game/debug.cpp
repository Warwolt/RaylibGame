#include "game/debug.h"

#include "game/animation.h"
#include "game/game.h"

#include <raylib.h>

using namespace std::chrono_literals;

void render_debug_overlay(const Game& game) {
	switch (game.debug.reload_state.value()) {
		case HotReloadState::Rebuilding: {
			const std::vector<AnimationFrame<std::string>> frames = {
				{ "Rebuilding", 500ms },
				{ "Rebuilding.", 500ms },
				{ "Rebuilding..", 500ms },
				{ "Rebuilding...", 500ms },
			};
			const std::string text = current_animation_frame(frames, game.debug.reload_state.last_changed(), Time::now());
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
