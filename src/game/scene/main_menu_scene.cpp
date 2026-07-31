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

void MenuUserInterface::initialize(Game* game) {
	m_images.focus_indicator = game->resources.load_image("resource/image/pointing_hand.png").value();
	m_sounds.menu_navigate = game->resources.load_sound("resource/sound/menu_navigate.wav").value();
}

void MenuUserInterface::menu_begin() {
	const ui::Style menu_begin_style = {
		.fit_content = true,
		.cross_alignment = ui::Alignment::Center,
	};
	UserInterface::box_begin(menu_begin_style);
}

bool MenuUserInterface::menu_item(const Input& input, const ResourceManager& resources, std::string_view label) {
	const ui::Style menu_item_style = {
		.position = ui::RelativePosition { .x = ui::Pixels(0), .y = ui::Pixels(0) },
		.fit_content = true,
		.direction = ui::Direction::Horizontal,
	};
	bool item_is_clicked = false;
	UserInterface::box_begin(menu_item_style);
	{
		const bool hovered_and_mouse = input.last_input_was_mouse() && UserInterface::element_is_hovered();
		const bool keyboard_or_gamepad = input.last_input_was_keyboard() || input.last_input_was_gamepad();
		const bool focused_and_keyboard_or_gamepad = keyboard_or_gamepad && UserInterface::element_is_focused();

		/* On click */
		item_is_clicked = UserInterface::element_is_clicked();

		/* On hover */
		if (UserInterface::element_is_hovered().has_changed_to(true)) {
			UserInterface::focus_current_element();
		}

		/* On focus */
		if (UserInterface::element_is_focused().has_changed_to(true)) {
			Raylib_PlaySound(resources.get_sound(m_sounds.menu_navigate));
		}

		/* Focus indicator */
		const double time_now = input.time_now.in_seconds();
		const double period = 1.8; // seconds
		const double freq = 1.0 / period;
		const double two_pi = 2.0 * std::numbers::pi;
		const float focus_indicator_offset = -10.0f * std::abs(std::cos(time_now * two_pi * freq));
		const int focus_indicator_size = 48;
		const ui::Style focus_indicator_style = {
			.position =
				ui::AbsolutePosition {
					.x = ui::Pixels(-focus_indicator_size + focus_indicator_offset),
					.y = ui::Pixels(-8),
				},
			.width = ui::Pixels(focus_indicator_size),
			.height = ui::Pixels(focus_indicator_size),
		};
		if (hovered_and_mouse || focused_and_keyboard_or_gamepad) {
			UserInterface::image(m_images.focus_indicator, focus_indicator_style);
		}

		/* Menu text */
		const ui::Style item_label_style = {
			.width = ui::Pixels(130),
			.padding = {
				.bottom = 2,
			},
			.alignment = ui::Alignment::Center,
			.font = {
				.size = 32,
				.color = (hovered_and_mouse || focused_and_keyboard_or_gamepad) ? YELLOW : WHITE,
			},
		};
		UserInterface::text(label, item_label_style);
	}
	UserInterface::box_end();
	return item_is_clicked;
}

void MenuUserInterface::menu_end() {
	UserInterface::box_end();
}

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
		game->scenes.pop_scene(game);
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
					.bottom = 48,
				},
				.font = {
					.size = 64,
				},
			};
			m_ui.text("Video Game", menu_title_style);

			/* Menu */
			m_ui.menu_begin();
			{
				m_ui.initially_focus_next_element();
				if (m_ui.menu_item(game->input, game->resources, "Start")) {
					game->scenes.push_scene(game, SceneID::Gameplay);
				}

				if (m_ui.menu_item(game->input, game->resources, "Settings")) {
					// TODO
				}

				if (m_ui.menu_item(game->input, game->resources, "Quit")) {
					game->scenes.pop_scene(game);
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
