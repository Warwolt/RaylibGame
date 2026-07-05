#include "game/scene/main_menu_scene.h"

#include "game/game.h"
#include "game/scene/scene_id.h"
#include "game/ui.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"

#include <raylib.h>
#include <raymath.h>

void MainMenuScene::initialize(Game* /*game*/) {
}

void MainMenuScene::deinitialize(Game* /*game*/) {
}

void MainMenuScene::update(Game* game) {
	PROFILING_SCOPE();
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->scenes.pop_scene(game);
	}
	if (Raylib_IsKeyPressed(KEY_ENTER)) {
		game->scenes.push_scene(game, SceneID::Gameplay);
	}

	m_ui.frame_begin();
	{
		ui::Style style = {
			.padding =
				ui::Spacing {
					.top = 2,
					.left = 10,
				},
			.alignment = ui::Alignment::Center,
			.font_size = 32,
		};
		m_ui.text("Continue", style);
		m_ui.text("Load Game", style);
		m_ui.text("New Game", style);
		m_ui.text("Settings", style);
		m_ui.text("Quit", style);
	}
	m_ui.frame_end(game->resources, game->window.size());
}

void MainMenuScene::render(const Game& game) const {
	PROFILING_SCOPE();
	Raylib_ClearBackground(BLACK);

	/* Render elements */
	m_ui.draw(game.resources);
}
