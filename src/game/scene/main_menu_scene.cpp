#include "game/scene/main_menu_scene.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"

#include "game/game.h"
#include "game/resource.h"
#include "game/scene/scene_id.h"
#include "game/ui/user_interface.h"

#include <raylib.h>
#include <raymath.h>

#include <cmath>
#include <numbers>
#include <unordered_map>
#include <vector>

void MainMenuScene::initialize(Game* game) {
	// clang-format off
	m_images.final_fantasy_menu_border = game->resources.load_image("resource/image/final_fantasy_menu_border_15_15.png").value();
	m_images.focus_indicator = game->resources.load_image("resource/image/pointing_hand.png").value();

	m_sounds.menu_navigate = game->resources.load_sound("resource/sound/menu_navigate.wav").value();
	// clang-format on
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
		game->scenes.pop_scene(game);
	}

	const double time_now = game->input.time_now.in_seconds();
	const double period = 1.8; // seconds
	const double freq = 1.0 / period;
	const double two_pi = 2.0 * std::numbers::pi;
	const float focus_indicator_offset = -10.0f * std::abs(std::cos(time_now * two_pi * freq));
	const int focus_indicator_size = 48;
	struct Styles {
		ui::Style menu_container;
		ui::Style menu_title;
		ui::Style item_list;
		ui::Style menu_item;
		ui::Style focus_indicator;
		ui::Style item_label;
	};
	const Styles styles = {
		.menu_container = {
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
		},
		.menu_title = {
			.margin = {
				.bottom = 48,
			},
			.font = {
				.size = 64,
			},
		},
		.item_list = {
			.fit_content = true,
			.cross_alignment = ui::Alignment::Center,
		},
		.menu_item = {
			.position = ui::RelativePosition { .x = ui::Pixels(0), .y = ui::Pixels(0) },
			.fit_content = true,
			.direction = ui::Direction::Horizontal,
		},
		.focus_indicator = {
			.position =
				ui::AbsolutePosition {
					.x = ui::Pixels(-focus_indicator_size + focus_indicator_offset),
					.y = ui::Pixels(-8),
				},
			.width = ui::Pixels(focus_indicator_size),
			.height = ui::Pixels(focus_indicator_size),
		},
		.item_label = {
			.width = ui::Pixels(130),
			.padding = {
				.bottom = 2,
			},
			.alignment = ui::Alignment::Center,
			.font = {
				.size = 32,
			},
		},
	};

	m_ui.frame_begin();
	{
		m_ui.box_begin(styles.menu_container);
		{
			/* Title */
			m_ui.text("Video Game", styles.menu_title);

			/* Menu */
			m_ui.box_begin(styles.item_list);
			{
				/* Items */
				for (MenuItem item : menu_items) {
					m_ui.box_begin(styles.menu_item);
					{
						m_ui.initially_focus_current_element();
						const bool hovered_and_mouse = game->input.last_input_was_mouse() && m_ui.element_is_hovered();
						const bool keyboard_or_gamepad = game->input.last_input_was_keyboard() || game->input.last_input_was_gamepad();
						const bool focused_and_keyboard_or_gamepad = keyboard_or_gamepad && m_ui.element_is_focused();

						/* On click */
						if (m_ui.element_is_clicked()) {
							switch (item) {
								case MenuItem_Start: {
									game->scenes.push_scene(game, SceneID::Gameplay);
								} break;

								case MenuItem_Settings: {
									/* Nothing yet */
								} break;

								case MenuItem_Quit: {
									game->scenes.pop_scene(game);
								} break;
							}
						}

						/* On hover */
						if (m_ui.element_is_hovered().has_changed_to(true)) {
							m_ui.focus_current_element();
						}

						/* On focus */
						if (m_ui.element_is_focused().has_changed_to(true)) {
							Raylib_PlaySound(game->resources.get_sound(m_sounds.menu_navigate));
						}

						/* Focus indicator */
						if (hovered_and_mouse || focused_and_keyboard_or_gamepad) {
							m_ui.image(m_images.focus_indicator, styles.focus_indicator);
						}

						/* Menu text */
						ui::Style item_label_style = styles.item_label;
						if (hovered_and_mouse || focused_and_keyboard_or_gamepad) {
							item_label_style.font.color = YELLOW;
						} else {
							item_label_style.font.color = WHITE;
						}
						m_ui.text(item_labels.at(item), item_label_style);
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
