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
}

void DebugScene::deinitialize(Game* /*game*/) {
}

void DebugScene::update(Game* game) {
	if (game->input.action_pressed(InputAction::ACTION_UI_BACK)) {
		game->scenes.queue_pop_scene();
	}

	m_line_segment.start = Vector2 { 384 / 4, 216 / 4 };
	m_line_segment.end = Vector2 { 3 * 384 / 4, 3 * 216 / 4 };

	const Rectangle start_vertex_box = vertex_rectangle(m_line_segment.start);
	const Rectangle end_vertex_box = vertex_rectangle(m_line_segment.end);
	m_start_hovered = Raylib_CheckCollisionPointRec(game->input.mouse_position, start_vertex_box);
	m_end_hovered = Raylib_CheckCollisionPointRec(game->input.mouse_position, end_vertex_box);
}

void DebugScene::render(const Game& /*game*/) const {
	const Rectangle start_vertex_box = vertex_rectangle(m_line_segment.start);
	const Rectangle end_vertex_box = vertex_rectangle(m_line_segment.end);
	Raylib_DrawLineV(m_line_segment.start, m_line_segment.end, GREEN);
	Raylib_DrawRectangleLinesEx(start_vertex_box, 1, m_start_hovered ? YELLOW : GREEN);
	Raylib_DrawRectangleLinesEx(end_vertex_box, 1, m_end_hovered ? YELLOW : GREEN);
}
