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
		ui::Style menu_style = {
			.alignment = ui::Alignment::Center,
		};
		ui::Style item_style = {
			.padding = ui::Spacing { .bottom = 2 },
			.alignment = ui::Alignment::Center,
			.font_size = 32,
		};
		ui::Style title_style = {
			.padding = ui::Spacing { .bottom = 32 },
			.alignment = ui::Alignment::Center,
			.font_size = 64,
		};
		m_ui.box_begin(menu_style);
		{
			m_ui.text("Raylib Game", title_style);

			m_ui.text("Continue", item_style);
			m_ui.text("Load Game", item_style);
			m_ui.text("New Game", item_style);
			m_ui.text("Settings", item_style);
			m_ui.text("Quit", item_style);
		}
		m_ui.box_end();
	}
	m_ui.frame_end(game->resources, game->window.size());
}

void MainMenuScene::render(const Game& game) const {
	PROFILING_SCOPE();
	Raylib_ClearBackground(BLACK);
	m_ui.draw(game.resources);
}
