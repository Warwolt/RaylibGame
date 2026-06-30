#include "game/scene/main_menu.h"

#include "game/game.h"
#include "game/ui.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"

#include <raylib.h>
#include <raymath.h>

void MainMenuScene::initialize(Game* /*game*/) {
	m_root_element = {
		.style = {
			.width = ui::RelativeSize(100),
			.height = ui::RelativeSize(100),
			.margin = ui::Spacing::uniform(0),
			.alignment = ui::Alignment::Start,
		},
		.content = ui::Box {
			.direction = ui::Direction::Vertical,
			.children = {
				ui::Element {
					.style = {
						.height = ui::RelativeSize(100),
					},
					.content = ui::Box {
						.direction = ui::Direction::Horizontal,
						.children = {
							ui::Element {
								.style = {
									.width = ui::RelativeSize(100),
									.border = ui::Spacing::uniform(5),
									.padding = ui::Spacing::uniform(10),
									.alignment = ui::Alignment::Start,
									.border_color = BLUE,
									.font_color = WHITE,
								},
								.content = ui::Box {
									.direction = ui::Direction::Vertical,
									.children = {
										ui::Element {
											.style = {
												.margin {
													.bottom = 10,
												},
											},
											.content = ui::Text {
												.text = "Samus Aran brings the last Metroid to the Ceres space colony for scientific study. Investigation of the specimen, a larva, reveals that its energy-producing abilities could be harnessed for the good of civilization.",
											},
										},
										ui::Element {
											.style = {
												.margin = {
													.bottom = 10,
												},
											},
											.content = ui::Text {
												.text = "Shortly after leaving, Samus receives a distress call alerting her to return to the colony immediately. She finds the scientists dead, and the Metroid larva stolen by Ridley, leader of the Space Pirates. Samus escapes during a self-destruct sequence and follows Ridley to the planet Zebes. She searches the planet for the Metroid and finds that the Pirates have rebuilt their base there.",
											},
										},
									},
								}
							},
							ui::Element {
								.style = {
									.width = ui::RelativeSize(100),
									.border = ui::Spacing::uniform(5),
									.padding = ui::Spacing::uniform(10),
									.border_color = GREEN,
									.font_color = WHITE,
								},
								.content = ui::Box {
									.children = {
										ui::Element {
											.style = {
												.margin = {
													.bottom = 10,
												}
											},
											.content = ui::Text {
												.text = "After defeating three bosses in various regions of Zebes, Samus confronts Ridley in his lair and defeats him, only to discover that the capsule containing the Metroid larva has been shattered and the larva is missing."
											},
										},
										ui::Element {
											.style = {
												.margin = {
													.bottom = 10,
												}
											},
											.content = ui::Text {
												.text = "She then heads for Tourian, the heart of the Space Pirates' base, and fights several Metroids that have reproduced. Samus confronts the Metroid larva, which has grown to enormous size. It attacks and nearly kills Samus, but relents at the last moment. As Samus was present at its hatching on SR388, the Metroid has imprinted on Samus, and recognizes her as its \"mother\"."
											},
										},
									},
								},
							},
						},
					},
				},
				ui::Element {
					.style = {
						.height = ui::RelativeSize(25),
						.border = ui::Spacing::uniform(5),
						.padding = ui::Spacing::uniform(10),
						.border_color = RED,
						.font_color = WHITE,
					},
					.content = ui::Box {
						.children = {
							ui::Element {
								.style = {
									.debug ={
										.show_content_outline = false,
									},
								},
								.content = ui::Text {
									.text = "> All systems nominal"
								},
							},
						},
					},
				},
			},
		},
	};
}

void MainMenuScene::deinitialize(Game* /*game*/) {
}

void MainMenuScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->scenes.pop_scene(game);
	}
	if (Raylib_IsKeyPressed(KEY_ENTER)) {
		game->scenes.push_scene(game, SceneID::Gameplay);
	}

	/* Compute layout */
	ui::layout_element(game->resources, game->window.size(), &m_root_element);

	{
		ui::ButtonState left_mouse_button = {};
		if (Raylib_IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			left_mouse_button = Raylib_IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? ui::ButtonState::Pressed : ui::ButtonState::Down;
		} else {
			left_mouse_button = Raylib_IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ? ui::ButtonState::Released : ui::ButtonState::Up;
		}

		const Rectangle letterbox = game->window.letterbox();
		const int letterbox_scale = game->window.letterbox_scale();
		const Vector2 mouse_pos = {
			.x = (Raylib_GetMouseX() - letterbox.x) / letterbox_scale,
			.y = (Raylib_GetMouseY() - letterbox.y) / letterbox_scale,
		};

		ui::Input input = {
			.mouse_pos = mouse_pos,
			.left_mouse_button = left_mouse_button,
		};

		ui::update_element(input, &m_root_element);

		if (left_mouse_button == ui::ButtonState::Pressed) {
			LOG_DEBUG("Left mouse pressed");
		}

		if (m_root_element.box()->children[0].box()->children[0].state.is_clicked) {
			game->scenes.push_scene(game, SceneID::Gameplay);
		}
	}
}

void MainMenuScene::render(const Game& game) const {
	Raylib_ClearBackground(BLACK);

	/* Render elements */
	ui::draw_element(game.resources, m_root_element);
}
