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

static Rectangle rectangle_centered_on_vertex(Vector2 vertex) {
	return centered_rectangle(Rectangle { 0, 0, VERTEX_SIZE, VERTEX_SIZE }, vertex);
}

static Vector2 Vector2Orthogonal(Vector2 v) {
	return Vector2 { -v.y, v.x };
}

std::optional<Vector2> segment_segment_intersection(LineSegment a, LineSegment b) {
	const Vector2 delta_a = a.end - a.start;
	const Vector2 delta_b = b.end - b.start;
	const Vector2 delta_a_orthogonal = Vector2Orthogonal(delta_a);
	const Vector2 delta_b_orthogonal = Vector2Orthogonal(delta_b);
	const Vector2 delta_ab = b.start - a.start;
	const Vector2 delta_ba = a.start - b.start;

	// Check if segments are parallel
	if (Vector2DotProduct(delta_a, delta_b_orthogonal) == 0) {
		return {};
	}

	// Compute segment parameter values t and u
	const float t = Vector2DotProduct(delta_ab, delta_b_orthogonal) / Vector2DotProduct(delta_a, delta_b_orthogonal);
	const float u = Vector2DotProduct(delta_ba, delta_a_orthogonal) / Vector2DotProduct(delta_b, delta_a_orthogonal);

	// Segments intersect if parameters t and u are both in range [0, 1]
	const bool t_in_range = 0.0f <= t && t <= 1.0f;
	const bool u_in_range = 0.0f <= u && u <= 1.0f;
	if (!t_in_range || !u_in_range) {
		return {};
	}

	// Return point of intersection
	return a.start + t * delta_a;
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
		const Rectangle vertex_box = rectangle_centered_on_vertex(vertex);

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

	// Things to do:
	//
	// We want to the collision between two rectangles (bounding boxes).
	// One or both of the rectangles should be moving with some delta vector.
	//
	// Each rectangle is made up of four line segments, and we want to find any
	// intersection between those rectangle sides.
	//
	// - Move line intersection code into own function
	// 		- That function should also handle parallel/colinear segments
	// - Add static and moveable rectangle to debug scene
	// 		- pressing LMB and dragging spans delta vector for first rectangle
	// 		- Releasing LMB moves first rectangle onto its resolved position

	/* Compute intersections */
	m_intersection = segment_segment_intersection(m_line_segments[0], m_line_segments[1]);
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
		Raylib_DrawRectangleLinesEx(rectangle_centered_on_vertex(vertex), 1, vertex_is_active ? YELLOW : GREEN);
	}

	// draw intersections
	if (m_intersection.has_value()) {
		Raylib_DrawPixelV(*m_intersection, RED);
		Raylib_DrawRectangleLinesEx(rectangle_centered_on_vertex(*m_intersection), 1, RED);
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
