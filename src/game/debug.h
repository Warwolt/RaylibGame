#pragma once

#include <core/util/timestamped.h>

struct Game;

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

struct Debug {
	Timestamped<HotReloadState> reload_state;
};

void render_debug_overlay(const Game& game);
