#pragma once

struct Game;

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

struct Debug {
	HotReloadState reload_state;
};

void render_debug_overlay(const Game& game);
