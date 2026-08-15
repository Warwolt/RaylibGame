#pragma once

#include "game/animation.h"

#include <string>

struct Game;

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

struct DebugOverlay {
	Animation<std::string> overlay_text_animation;
};

void initialize_debug_overlay(Game* game);
void render_debug_overlay(const Game& game);
