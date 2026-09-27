#include "game/scene/debug_scene.h"

#include "game/game.h"

#include <raymath.h>

#include "core/debug/assert.h"
#include "core/debug/logging.h"

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
	const std::optional<size_t> selected_rectangle_index = m_selected_rectangle_index; // GROSS HACK YUCK
	bool some_rectangle_is_active = false;
	for (size_t i = 0; i < m_rectangles.size(); i++) {
		const Rectangle& rectangle = m_rectangles[i];
		InteractionState& rectangle_state = m_rectangle_states[i];
		rectangle_state.is_hovered = Raylib_CheckCollisionPointRec(game->input.mouse_position, rectangle);

		if (game->input.left_mouse_button_pressed()) {
			if (rectangle_state.is_hovered) {
				// rectangle selected
				rectangle_state.is_active = !rectangle_state.is_active;
				some_rectangle_is_active |= true;
				m_selected_rectangle_index = i;
			} else {
				// rectangle unselected
				rectangle_state.is_active = false;
				if (m_selected_rectangle_index == i) {
					m_selected_rectangle_index = std::nullopt;
				}
			}
		}
	}

	/* Delta line */
	// on rectangle clicked
	if (game->input.left_mouse_button_pressed()) {
		if (m_delta_start.has_value()) {
			const Vector2 delta_end = game->input.mouse_position;
			const Vector2 delta = delta_end - *m_delta_start;
			m_delta_start = std::nullopt;

			// move selected rectangle by delta
			ASSERT(selected_rectangle_index.has_value(), "Some rectangle should be selected");
			Rectangle& selected_rectangle = m_rectangles[*selected_rectangle_index];
			selected_rectangle = selected_rectangle + delta;

		} else if (some_rectangle_is_active) {
			m_delta_start = game->input.mouse_position;
		}
	}
}

void DebugScene::render(const Game& game) const {
	for (size_t i = 0; i < m_rectangles.size(); i++) {
		const Rectangle& rectangle = m_rectangles[i];
		Raylib_DrawRectangleLinesEx(rectangle, 1.0f, GREEN);
	}

	if (m_delta_start.has_value()) {
		Raylib_DrawLineV(m_delta_start.value(), game.input.mouse_position, YELLOW);
	}
}
