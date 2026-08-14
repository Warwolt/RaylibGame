#pragma once

#include "core/util/tracked.h"
#include "game/animation.h"

struct Game;

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

struct Debug {
	Tracked<HotReloadState> reload_state;
	AnimatedValue<std::string> overlay_text_animation;
};

void initialize_debug_overlay(Game* game);
void update_debug_overlay(Game* game);
void render_debug_overlay(const Game& game);
