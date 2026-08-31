#include "game/scene/debug_scene.h"

#include "game/game.h"

#include <raylib.h>
#include <raymath.h>

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
	/* Quit */
	if (game->input.action_pressed(InputAction::ACTION_UI_BACK)) {
		game->scenes.queue_pop_scene();
	}

	/* Grab line segments */
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

	/* Compute intersections */
	{
		m_intersection = {};
		const float x1 = m_line_segments[0].start.x;
		const float y1 = m_line_segments[0].start.y;
		const float x2 = m_line_segments[0].end.x;
		const float y2 = m_line_segments[0].end.y;
		const float x3 = m_line_segments[1].start.x;
		const float y3 = m_line_segments[1].start.y;
		const float x4 = m_line_segments[1].end.x;
		const float y4 = m_line_segments[1].end.y;
		const float t_denominator = (x2 - x1) * (y3 - y4) + (y2 - y1) * (x4 - x3);
		const float u_denominator = (x4 - x3) * (y1 - y2) + (y4 - y3) * (x2 - x1);
		if (t_denominator != 0 && u_denominator != 0) {
			const float t_numerator = (x3 - x1) * (y3 - y4) + (y3 - y1) * (x4 - x3);
			const float u_numerator = (x1 - x3) * (y1 - y2) + (y1 - y3) * (x2 - x1);
			const float t = t_numerator / t_denominator;
			const float u = u_numerator / u_denominator;
			if (0.0f <= t && t <= 1.0f && 0.0f <= u && u <= 1.0f) {
				const LineSegment& segment = m_line_segments[0];
				m_intersection = segment.start + t * (segment.end - segment.start);
			}
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

	// draw intersections
	if (m_intersection.has_value()) {
		Raylib_DrawPixelV(*m_intersection, RED);
		Raylib_DrawRectangleLinesEx(vertex_rectangle(*m_intersection), 1, RED);
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
