#include "game/scene/debug_scene.h"

#include "game/game.h"

#include <raylib.h>

constexpr int VERTEX_SIZE = 6;

static Rectangle centered_rectangle(Rectangle rect, Vector2 center) {
	return Rectangle {
		.x = center.x - rect.width / 2,
		.y = center.y - rect.height / 2,
		.width = rect.width,
		.height = rect.height,
	};
}

static Rectangle vertex_rectangle(Vector2 vertex) {
	return centered_rectangle(Rectangle { 0, 0, VERTEX_SIZE, VERTEX_SIZE }, vertex);
}

void DebugScene::initialize(Game* /*game*/) {
	m_line_segments = {
		LineSegment {
			.start = Vector2 { 384 / 4, 216 / 4 },
			.end = Vector2 { 3 * 384 / 4, 3 * 216 / 4 },
		},
		LineSegment {
			.start = Vector2 { 130, 120 },
			.end = Vector2 { 250, 100 },
		},
	};

	m_vertex_states.resize(2 * m_line_segments.size());
}

void DebugScene::deinitialize(Game* /*game*/) {
}

void DebugScene::update(Game* game) {
	if (game->input.action_pressed(InputAction::ACTION_UI_BACK)) {
		game->scenes.queue_pop_scene();
	}

	for (int i = 0; i < m_line_segments.size() * 2; i++) {
		Vector2& vertex = _line_segment_vertex(i);
		VertexState& vertex_state = m_vertex_states[i];
		const Rectangle vertex_box = vertex_rectangle(vertex);

		vertex_state.is_hovered = Raylib_CheckCollisionPointRec(game->input.mouse_position, vertex_box);

		if (vertex_state.is_hovered && game->input.left_mouse_button_pressed()) {
			vertex_state.is_grabbed = true;
		}
		if (game->input.left_mouse_button_released()) {
			vertex_state.is_grabbed = false;
		}

		if (vertex_state.is_grabbed) {
			vertex = game->input.mouse_position;
		}
	}
}

void DebugScene::render(const Game& /*game*/) const {
	// draw lines
	for (const LineSegment& line_segment : m_line_segments) {
		Raylib_DrawLineV(line_segment.start, line_segment.end, GREEN);
	}

	// draw vertex boxes
	for (int i = 0; i < m_line_segments.size() * 2; i++) {
		const Vector2& vertex = _line_segment_vertex(i);
		const bool vertex_is_active = m_vertex_states[i].is_hovered || m_vertex_states[i].is_grabbed;
		Raylib_DrawRectangleLinesEx(vertex_rectangle(vertex), 1, vertex_is_active ? YELLOW : GREEN);
	}
}

Vector2& DebugScene::_line_segment_vertex(int index) {
	LineSegment& line_segment = m_line_segments[index / 2];
	return index % 2 == 0 ? line_segment.start : line_segment.end;
}

const Vector2& DebugScene::_line_segment_vertex(int index) const {
	const LineSegment& line_segment = m_line_segments[index / 2];
	return index % 2 == 0 ? line_segment.start : line_segment.end;
}
