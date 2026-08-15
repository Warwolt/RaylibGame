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

class DebugOverlay {
public:
	void initialize();
	void render(const Game& game) const;

private:
	Animation<std::string> m_overlay_text_animation;
};
