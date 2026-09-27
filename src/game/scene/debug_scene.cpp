#include "game/scene/debug_scene.h"

#include "game/game.h"

#include <raymath.h>

void DebugScene::initialize(Game* /*game*/) {
	m_rectangles = {
		Rectangle { 100, 100, 50, 50 },
		Rectangle { 200, 50, 50, 50 },
	};
}

void DebugScene::deinitialize(Game* /*game*/) {
}

void DebugScene::update(Game* game) {
	/* Quit */
	if (game->input.action_pressed(InputAction::ACTION_UI_BACK)) {
		game->scenes.queue_pop_scene();
	}
}

void DebugScene::render(const Game& /*game*/) const {
	for (Rectangle rectangle : m_rectangles) {
		Raylib_DrawRectangleLinesEx(rectangle, 1.0f, GREEN);
	}
}
