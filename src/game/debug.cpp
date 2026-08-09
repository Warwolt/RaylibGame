#include "game/debug.h"

#include "game/animation.h"
#include "game/game.h"

#include <raylib.h>

using namespace std::chrono_literals;

void render_debug_overlay(const Game& game) {
	switch (game.debug.reload_state.value()) {
		case HotReloadState::Rebuilding: {
			const FrameAnimation<std::string> overlay_text_animation = {
				.frames = {
					{ "Rebuilding", 500ms },
					{ "Rebuilding.", 500ms },
					{ "Rebuilding..", 500ms },
					{ "Rebuilding...", 500ms },
				},
				.start_time = game.debug.reload_state.last_changed(),
			};
			const std::string overlay_text = overlay_text_animation.current_frame(game.input.time_now);
			Raylib_DrawRectangle(0, 0, game.window.width(), game.window.height(), Color { 0, 0, 0, 127 });
			Raylib_DrawTextEx(game.resources.get_font(FontID::default_font()), overlay_text.c_str(), { 4, 0 }, 16, 0, YELLOW);
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
