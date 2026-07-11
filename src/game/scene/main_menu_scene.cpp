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
	m_images.developer_face = game->resources.load_image("resource/image/developer_face_1000_1000.png").value();
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
			.background = { .image = m_images.mario64_skybox },
		};
		ui::Style image_container_style = {
			.width = ui::Percentage(33),
			.border = {
				.edges = ui::Edges::uniform(16),
				.image = m_images.final_fantasy_menu_border,
				.image_slices = ui::Edges::uniform(5),
			},
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
			.background = { .color = Color { 20, 37, 136, 255 }, }

		};
		ui::Style image_style {
			.width = ui::Pixels(200),
			.height = ui::Pixels(200),
			.border = {
				.edges = ui::Edges::uniform(2),
				.color = WHITE,
			},
		};
		ui::Style item_style = {
			.padding = {
					.bottom = 2,
				},
			.alignment = ui::Alignment::Center,
			.font = {
				.size = 32,
				.color = WHITE,
			}
		};
		m_ui.box_begin(ui::Direction::Vertical, menu_style);
		{
			m_ui.box_begin(ui::Direction::Horizontal, ui::Style { .alignment = ui::Alignment::Center });
			{
				m_ui.box_begin(ui::Direction::Vertical, image_container_style);
				{
					m_ui.image(m_images.developer_face, image_style);
				}
				m_ui.box_end();
			}
			m_ui.box_end();

			m_ui.box_begin(
				ui::Direction::Vertical,
				ui::Style {
					.padding = ui::Edges::uniform(16),
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
