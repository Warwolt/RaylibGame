#include "game/scene/debug_scene.h"

#include "game/game.h"

#include <raylib.h>

void DebugScene::initialize(Game* /*game*/) {
}
void DebugScene::deinitialize(Game* /*game*/) {
}

void DebugScene::update(Game* game) {
	if (game->input.action_pressed(InputAction::ACTION_UI_BACK)) {
		game->scenes.queue_pop_scene();
	}
}

void DebugScene::render(const Game& /*game*/) const {
	//
}
