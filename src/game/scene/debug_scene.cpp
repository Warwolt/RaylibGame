#include "game/scene/debug_scene.h"

#include "game/game.h"

#include <raymath.h>

void DebugScene::initialize(Game* /*game*/) {
	m_rectangles = {
		Rectangle { 100, 100, 50, 50 },
		Rectangle { 200, 50, 50, 50 },
	};
	m_rectangle_states.resize(m_rectangles.size());
}

void DebugScene::deinitialize(Game* /*game*/) {
}

void DebugScene::update(Game* game) {
	/* Quit */
	if (game->input.action_pressed(InputAction::ACTION_UI_BACK)) {
		game->scenes.queue_pop_scene();
	}

	/* Rectangle interactivity */
	bool some_rectangle_is_hovered = false;
	for (size_t i = 0; i < m_rectangles.size(); i++) {
		const Rectangle& rectangle = m_rectangles[i];
		InteractionState& rectangle_state = m_rectangle_states[i];
		rectangle_state.is_hovered = Raylib_CheckCollisionPointRec(game->input.mouse_position, rectangle);
		some_rectangle_is_hovered |= rectangle_state.is_hovered;
	}

	if (game->input.left_mouse_button_pressed()) {
		if (m_delta_start.has_value()) {
			m_delta_start = std::nullopt; // TODO set end here
		} else if (some_rectangle_is_hovered) {
			m_delta_start = game->input.mouse_position;
		}
	}
}

void DebugScene::render(const Game& game) const {
	for (size_t i = 0; i < m_rectangles.size(); i++) {
		const Rectangle& rectangle = m_rectangles[i];
		const Color color = m_rectangle_states[i].is_hovered ? YELLOW : GREEN;
		Raylib_DrawRectangleLinesEx(rectangle, 1.0f, color);
	}

	if (m_delta_start.has_value()) {
		Raylib_DrawLineV(m_delta_start.value(), game.input.mouse_position, YELLOW);
	}
}
