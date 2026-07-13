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
	m_images.focus_indicator = game->resources.load_image("resource/image/pointing_hand.png").value();
	// clang-format on
}

void MainMenuScene::deinitialize(Game* /*game*/) {
}

namespace MenuItems {
	enum {
		Continue,
		LoadGame,
		NewGame,
		Settings,
		Quit,
		Count,
	};
}

void MainMenuScene::update(Game* game) {
	PROFILING_SCOPE();
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->scenes.pop_scene(game);
	}

	const char* menu_items[MenuItems::Count] = {};
	menu_items[MenuItems::Continue] = "Continue";
	menu_items[MenuItems::LoadGame] = "Load Game";
	menu_items[MenuItems::NewGame] = "New Game";
	menu_items[MenuItems::Settings] = "Settings";
	menu_items[MenuItems::Quit] = "Quit";

	if (Raylib_IsKeyPressed(KEY_DOWN)) {
		m_menu_index = (MenuItems::Count + m_menu_index + 1) % MenuItems::Count;
	}
	if (Raylib_IsKeyPressed(KEY_UP)) {
		m_menu_index = (MenuItems::Count + m_menu_index - 1) % MenuItems::Count;
	}
	if (Raylib_IsKeyPressed(KEY_ENTER)) {
		switch (m_menu_index) {
			case MenuItems::Continue: {
				game->scenes.push_scene(game, SceneID::Gameplay);
			} break;

			case MenuItems::Quit: {
				game->scenes.pop_scene(game);
			} break;
		}
	}

	const ui::Style menu_style = {
		.alignment = ui::Alignment::Center,
		.background = { .image = m_images.mario64_skybox },
	};
	const ui::Style image_container_style = {
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
	const ui::Style image_style {
			.width = ui::Pixels(200),
			.height = ui::Pixels(200),
			.border = {
				.edges = ui::Edges::uniform(2),
				.color = WHITE,
			},
		};
	const ui::Style menu_container = {
		.padding = ui::Edges::uniform(16),
		.alignment = ui::Alignment::Center,
		.cross_alignment = ui::Alignment::Center,
	};
	const ui::Style menu_item_container = {
		.position = ui::RelativePosition { .x = ui::Pixels(0), .y = ui::Pixels(0) },
		.fit_content = true,
	};
	const ui::Style menu_item_image_style = {
			.width = ui::Pixels(150),
			.padding = {
					.bottom = 2,
				},
			.alignment = ui::Alignment::Center,
			.font = {
				.size = 32,
				.color = WHITE,
			}
		};
	const int indicator_size = 48;
	const ui::Style focus_indicator_style = {
		.position =
			ui::AbsolutePosition {
				.x = ui::Pixels(-indicator_size),
				.y = ui::Pixels(-10),
			},
		.width = ui::Pixels(indicator_size),
		.height = ui::Pixels(indicator_size),
		.debug = {
			.show_content_outline = true,
		},
	};

	m_ui.frame_begin();
	{
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

			m_ui.box_begin(ui::Direction::Vertical, menu_container);
			{
				for (int i = 0; i < MenuItems::Count; i++) {
					m_ui.box_begin(ui::Direction::Horizontal, menu_item_container);
					{
						if (m_menu_index == i) {
							m_ui.image(m_images.focus_indicator, focus_indicator_style);
						}
						// FIXME: absolutely positioned image should be able to overflow it's container
						// but right now it gets clamped to the parent size.
						m_ui.text(menu_items[i], menu_item_image_style);
					}
					m_ui.box_end();
				}
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
