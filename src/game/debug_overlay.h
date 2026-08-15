#pragma once

#include "game/animation.h"

#include <raylib.h>

#include <string>

struct Game;

class DebugOverlay {
public:
	void initialize();
	void update(Game* game);
	void render(const Game& game) const;

private:
	std::string m_overlay_text;
	Color m_overlay_text_color = YELLOW;
	AnimationPlayer<std::string> m_overlay_text_animation;
};
