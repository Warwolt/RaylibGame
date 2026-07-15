#pragma once

#include <core/util/time.h>

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
