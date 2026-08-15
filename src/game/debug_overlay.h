#pragma once

#include "core/util/time.h"
#include "game/animation.h"

#include <string>

struct Game;

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

struct Debug {
	Timestamped<HotReloadState> reload_state;
	Animation<std::string> overlay_text_animation;
};

void initialize_debug_overlay(Game* game);
void render_debug_overlay(const Game& game);
