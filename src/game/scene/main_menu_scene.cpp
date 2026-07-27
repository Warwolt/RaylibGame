#include "game/scene/main_menu_scene.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"

#include "game/game.h"
#include "game/resource.h"
#include "game/scene/scene_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>
#include <raymath.h>

void MainMenuScene::initialize(Game* game) {
	// clang-format off
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
	if (game->input.key_pressed(KEY_ESCAPE)) {
		game->scenes.pop_scene(game);
	}

	const ui::Style menu_style = {
		.alignment = ui::Alignment::Center,
	};
	const ui::Style title_container_style = {
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
	const ui::Style menu_item_text_style = {
			.width = ui::Pixels(150),
			.padding = {
					.bottom = 2,
				},
			.alignment = ui::Alignment::Center,
			.font = {
				.size = 32,
				.color = WHITE,
			},
			.hover = {
				.font = {
					.color = YELLOW,
				},
			},
			.focus = {
				.font = {
					.color = YELLOW,
				},
			},
		};
	const int indicator_size = 48;
	const ui::Style focus_indicator_style = {
		.position =
			ui::AbsolutePosition {
				.x = ui::Pixels(-indicator_size),
				.y = ui::Pixels(-8),
			},
		.width = ui::Pixels(indicator_size),
		.height = ui::Pixels(indicator_size),
	};

	const char* menu_items[MenuItems::Count] = {};
	menu_items[MenuItems::Continue] = "Continue";
	menu_items[MenuItems::LoadGame] = "Load Game";
	menu_items[MenuItems::NewGame] = "New Game";
	menu_items[MenuItems::Settings] = "Settings";
	menu_items[MenuItems::Quit] = "Quit";
	m_ui.frame_begin();
	{
		m_ui.box_begin(ui::Direction::Vertical, menu_style);
		{
			m_ui.box_begin(ui::Direction::Horizontal, ui::Style { .alignment = ui::Alignment::Center });
			{
				m_ui.box_begin(ui::Direction::Vertical, title_container_style);
				{
					m_ui.text("Video Game", ui::Style { .alignment = ui::Alignment::Center, .font = { .size = 64 } });
				}
				m_ui.box_end();
			}
			m_ui.box_end();

			m_ui.box_begin(ui::Direction::Vertical, menu_container);
			{
				/* Menu */
				for (int i = 0; i < MenuItems::Count; i++) {
					/* Menu item */
					m_ui.box_begin(ui::Direction::Horizontal, menu_item_container, menu_items[i]);
					{
						m_ui.initially_focus_current_element();

						/* On click */
						if (m_ui.element_is_clicked()) {
							switch (i) {
								case MenuItems::Continue: {
									game->scenes.push_scene(game, SceneID::Gameplay);
								} break;

								case MenuItems::Quit: {
									game->scenes.pop_scene(game);
								} break;
							}
						}

						/* On hover */
						if (m_ui.element_is_hovered()) {
							m_ui.focus_current_element();
						}

						/* Focus indicator */
						const bool hovered_and_mouse = game->input.last_input_was_mouse() && m_ui.element_is_hovered();
						const bool focused_and_keyboard = game->input.last_input_was_keyboard() && m_ui.element_is_focused();
						if (hovered_and_mouse || focused_and_keyboard) {
							m_ui.image(m_images.focus_indicator, focus_indicator_style);
						}

						/* Menu text */
						m_ui.text(menu_items[i], menu_item_text_style);
					}
					m_ui.box_end();
				}
			}
			m_ui.box_end();
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
