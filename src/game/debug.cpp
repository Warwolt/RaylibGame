#include "game/debug.h"

#include "game/game.h"

#include <raylib.h>

#include <cmath>

struct AnimationFrame {
	std::string value;
	double duration; // seconds
};

std::string current_animation_frame(const std::vector<AnimationFrame>& frames, double animation_start, double time_now) {
	/* Compute animation period */
	double period = 0;
	for (const AnimationFrame& frame : frames) {
		period += frame.duration;
	}

	/* Find current frame */
	double frame_start = 0;
	const double playback_time = fmod(time_now - animation_start, period);
	for (const AnimationFrame& frame : frames) {
		if (frame_start <= playback_time && playback_time < frame_start + frame.duration) {
			return frame.value;
		}
		frame_start += frame.duration;
	}

	return "";
}

void render_debug_overlay(const Game& game) {
	const std::vector<AnimationFrame> frames = {
		{ "Rebuilding.", 0.5 },
		{ "Rebuilding..", 0.5 },
		{ "Rebuilding...", 0.5 },
	};
	const std::string text = current_animation_frame(frames, game.debug.reload_state.last_changed(), Raylib_GetTime());

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
