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
		LineSegment {
			.start = Vector2 { 150, 50 },
			.end = Vector2 { 180, 150 },
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

	/* Grab line segments with mouse button */
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
	m_intersections.clear();
	for (int i = 0; i < m_line_segments.size(); i++) {
		for (int j = i; j < m_line_segments.size(); j++) {
			if (std::optional<Vector2> intersection = segment_segment_intersection(m_line_segments[i], m_line_segments[j])) {
				m_intersections.push_back(*intersection);
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
		Raylib_DrawRectangleLinesEx(rectangle_centered_on_vertex(vertex), 1, vertex_is_active ? YELLOW : GREEN);
	}

	// draw intersections
	for (const Vector2& intersection : m_intersections) {
		Raylib_DrawPixelV(intersection, RED);
		Raylib_DrawRectangleLinesEx(rectangle_centered_on_vertex(intersection), 1, RED);
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
