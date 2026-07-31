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

void menu_begin(ui::UserInterface* ui) {
	const ui::Style menu_begin_style = {
		.fit_content = true,
		.cross_alignment = ui::Alignment::Center,
	};
	ui->box_begin(menu_begin_style);
}

bool menu_item(ui::UserInterface* ui, const Input& input, const ResourceManager& resources, std::string label) {
	const ui::Style menu_item_style = {
		.position = ui::RelativePosition { .x = ui::Pixels(0), .y = ui::Pixels(0) },
		.fit_content = true,
		.direction = ui::Direction::Horizontal,
	};
	bool item_is_clicked = false;
	ui->box_begin(menu_item_style);
	{
		const bool hovered_and_mouse = input.last_input_was_mouse() && ui->element_is_hovered();
		const bool keyboard_or_gamepad = input.last_input_was_keyboard() || input.last_input_was_gamepad();
		const bool focused_and_keyboard_or_gamepad = keyboard_or_gamepad && ui->element_is_focused();

		/* On click */
		item_is_clicked = ui->element_is_clicked();

		/* On hover */
		if (ui->element_is_hovered().has_changed_to(true)) {
			ui->focus_current_element();
		}

		/* On focus */
		if (ui->element_is_focused().has_changed_to(true)) {
			// FIXME: we need access to this sound somehow
			// Make this a class?
			// Raylib_PlaySound(resources.get_sound(m_sounds.menu_navigate));
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
			// FIXME: we need access to this image somehow
			// Make this a class?
			//ui->image(m_images.focus_indicator, focus_indicator_style);
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
		ui->text(label.c_str(), item_label_style);
	}
	ui->box_end();
	return item_is_clicked;
}

void menu_end(ui::UserInterface* ui) {
	ui->box_end();
}

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
			menu_begin(&m_ui);
			{
				if (menu_item(&m_ui, game->input, game->resources, "Start")) {
					game->scenes.push_scene(game, SceneID::Gameplay);
				}

				if (menu_item(&m_ui, game->input, game->resources, "Settings")) {
					// TODO
				}

				if (menu_item(&m_ui, game->input, game->resources, "Quit")) {
					game->scenes.pop_scene(game);
				}
			}
			menu_end(&m_ui);
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
