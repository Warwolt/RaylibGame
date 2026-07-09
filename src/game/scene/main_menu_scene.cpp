#include "game/scene/main_menu_scene.h"

#include "core/debug/profiling.h"

#include "game/game.h"
#include "game/resource.h"
#include "game/scene/scene_id.h"
#include "game/ui.h"

#include <raylib.h>
#include <raymath.h>

void MainMenuScene::initialize(Game* game) {
	// clang-format off
	m_images.mario64_skybox = game->resources.load_image("resource/image/mario64_skybox.jpg").value();
	m_images.utah_teapot = game->resources.load_image("resource/image/test/utah_teapot_1000_818.png").value();
	m_images.final_fantasy_menu_border = game->resources.load_image("resource/image/final_fantasy_menu_border_15_15.png").value();
	// clang-format on
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
			.background_image = m_images.mario64_skybox,
		};
		ui::Style image_container_style = {
			.border = ui::Spacing::uniform(16),
			.alignment = ui::Alignment::Center,
			.background_color = Color { 20, 37, 136, 255 },
			.border_image = m_images.final_fantasy_menu_border,
			.border_image_slicing = ui::Spacing::uniform(5),
		};
		ui::Style image_style {
			.width = ui::AbsoluteSize(250),
			.height = ui::AbsoluteSize(200),
		};
		ui::Style item_style = {
			.padding = {
					.bottom = 2,
				},
			.alignment = ui::Alignment::Center,
			.font_color = WHITE,
			.font_size = 32,
		};
		m_ui.box_begin(ui::Direction::Vertical, menu_style);
		{
			m_ui.box_begin(ui::Direction::Horizontal, image_container_style);
			{
				m_ui.image(m_images.utah_teapot, image_style);
			}
			m_ui.box_end();

			m_ui.box_begin(
				ui::Direction::Vertical,
				ui::Style {
					.padding = ui::Spacing::uniform(16),
				}
			);
			{
				m_ui.text("Continue", item_style);
				m_ui.text("Load Game", item_style);
				m_ui.text("New Game", item_style);
				m_ui.text("Settings", item_style);
				m_ui.text("Quit", item_style);
			}
			m_ui.box_end();
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
