#include "game/scene/main_menu_scene.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"

#include "game/game.h"
#include "game/resource.h"
#include "game/scene/scene_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>
#include <raymath.h>

#include <unordered_map>
#include <vector>

void MainMenuScene::initialize(Game* game) {
	m_ui.initialize(game);
}

void MainMenuScene::deinitialize(Game* /*game*/) {
}

enum MenuItem {
	MenuItem_Start,
	MenuItem_Settings,
	MenuItem_Quit,
};

const std::vector<MenuItem> menu_items = {
	MenuItem_Start,
	MenuItem_Settings,
	MenuItem_Quit,
};

const std::unordered_map<MenuItem, const char*> item_labels = {
	{ MenuItem_Start, "Start" },
	{ MenuItem_Settings, "Settings" },
	{ MenuItem_Quit, "Quit" },
};

void MainMenuScene::update(Game* game) {
	PROFILING_SCOPE();
	if (game->input.key_pressed(KEY_ESCAPE)) {
		game->scenes.queue_pop_scene();
	}

	m_ui.frame_begin();
	{
		const ui::Style centered_container_style = {
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
		};
		m_ui.box_begin(centered_container_style);
		{
			/* Title */
			const ui::Style menu_title_style = {
				.margin = {
					.bottom = 24,
				},
				.font = {
					.size = 64,
				},
			};
			m_ui.text("Video Game", menu_title_style);

			/* Menu */
			m_ui.menu_begin();
			{
				if (m_ui.menu_item(game->input, game->resources, "Start")) {
					game->scenes.queue_push_scene(SceneID::Gameplay);
				}

				if (m_ui.menu_item(game->input, game->resources, "Settings")) {
					// TODO
				}

				if (m_ui.menu_item(game->input, game->resources, "Quit")) {
					game->scenes.queue_pop_scene();
				}
			}
			m_ui.menu_end();
		}
		m_ui.box_end();
	}
	m_ui.frame_end(game->input, game->resources, game->window.size());
}

void MainMenuScene::render(const Game& game) const {
	PROFILING_SCOPE();
	Raylib_ClearBackground(BLACK);
	m_ui.draw(game.resources);
}
