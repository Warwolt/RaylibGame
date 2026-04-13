#include "game/ui.h"

#include "core/debug/logging.h"
#include "test/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <filesystem>
#include <functional>

constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;
constexpr Vector2 SCREEN_SIZE = { SCREEN_WIDTH, SCREEN_HEIGHT };
#define LIGHTGREEN Color(191, 240, 172, 255)

const ui::Style button_style = {
		.width = ui::AbsoluteSize(200),
		.height = ui::AbsoluteSize(100),
		.border = ui::Spacing::uniform(10),
		.padding = ui::Spacing::uniform(10),
		.alignment = ui::Alignment::Center,
		.border_color = DARKGREEN,
		.background_color = GREEN,
		.font_color = DARKGREEN,

		.hovered = {
			.border_color = GREEN,
			.background_color = LIGHTGREEN,
		},

		.active = {
			.border_color = GREEN,
			.background_color = DARKGREEN,
			.font_color = GREEN,
		},
	};

class UILayoutTests : public ::testing::Test {
public:
	ResourceManager m_resources;

	void SetUp() {
		Raylib_SetTraceLogLevel(LOG_WARNING);
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN);
		Raylib_InitWindow(1, 1, "Unit Test");
		m_resources.load_default_font("resource/font/ModernDOS8x16.ttf");
	}

	void TearDown() {
		Raylib_CloseWindow();
	}
};

TEST_F(UILayoutTests, BoxLayout_100_100_Gives_50_50) {
	ui::Element element = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = RED,
					},
				},
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = GREEN,
					},
				}
			}
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_EQ(std::get<ui::Box>(element.content).children[0].layout.margin_box.width, SCREEN_WIDTH / 2);
	EXPECT_EQ(std::get<ui::Box>(element.content).children[1].layout.margin_box.width, SCREEN_WIDTH / 2);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, BoxLayout_100_25_100_Gives_37_25_37) {
	ui::Element element = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = RED,
					},
				},
				ui::Element {
					.style = {
						.width = ui::RelativeSize(25),
						.background_color = GREEN,
					},
				},
				ui::Element {
					.style = {
						.width = ui::RelativeSize(100),
						.background_color = BLUE,
					},
				}
			}
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_EQ(std::get<ui::Box>(element.content).children[0].layout.margin_box.width, SCREEN_WIDTH * 0.375);
	EXPECT_EQ(std::get<ui::Box>(element.content).children[1].layout.margin_box.width, SCREEN_WIDTH * 0.25);
	EXPECT_EQ(std::get<ui::Box>(element.content).children[2].layout.margin_box.width, SCREEN_WIDTH * 0.375);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, BoxLayout_DefaultStyle) {
	ui::Element element = {
		.style = button_style,
		.content = ui::Text { .text = "Press Me" },
	};
	element.state.is_hovered = false;
	element.state.is_active = false;

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, BoxLayout_HoveredStyle) {
	ui::Element element = {
		.style = button_style,
		.content = ui::Text { .text = "Press Me" },
	};
	element.state.is_hovered = true;
	element.state.is_active = false;

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, BoxLayout_ActiveStyle) {
	ui::Element element = {
		.style = button_style,
		.content = ui::Text { .text = "Press Me" },
	};
	element.state.is_hovered = true;
	element.state.is_active = true;

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, Text_LeftAligned) {
	ui::Element element = {
		.style = {
			.width = ui::RelativeSize(100),
			.padding = ui::Spacing::uniform(20),
			.alignment = ui::Alignment::Start,
			.font_size = 32,
		},
		.content = ui::Text {
			.text = "Left Aligned Text",
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, Text_CenterAligned) {
	ui::Element element = {
		.style = {
			.width = ui::RelativeSize(100),
			.padding = ui::Spacing::uniform(20),
			.alignment = ui::Alignment::Center,
			.font_size = 32,
		},
		.content = ui::Text {
			.text = "Center Aligned Text",
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, Text_RightAligned) {
	ui::Element element = {
		.style = {
			.width = ui::RelativeSize(100),
			.padding = ui::Spacing::uniform(20),
			.alignment = ui::Alignment::End,
			.font_size = 32,
		},
		.content = ui::Text {
			.text = "Right Aligned Text",
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UILayoutTests, Text_MultipleParagraphs_WithTitle) {
	ui::Element element = {
		.style = {
			.width = ui::RelativeSize(100),
			.padding = ui::Spacing::uniform(20),
		},
		.content = ui::Box {
			.direction = ui::Direction::Vertical,
			.children = {
				ui::Element {
					.style = {
						.margin = {
							.bottom = 16,
						},
						.font_size = 32,
					},
					.content = ui::Text {
						.text = "Super Metroid",
					}
				},
				ui::Element {
					.style = {
						.margin = {
							.bottom = 16,
						},
					},
					.content = ui::Text {
						.text = "Samus Aran brings the last Metroid to the Ceres space colony for scientific study. Investigation of the specimen, a larva, reveals that its energy-producing abilities could be harnessed for the good of civilization.",
					}
				},
				ui::Element {
					.style = {
						.margin = {
							.bottom = 16,
						},
					},
					.content = ui::Text {
						.text = "Shortly after leaving, Samus receives a distress call alerting her to return to the colony immediately. She finds the scientists dead, and the Metroid larva stolen by Ridley, leader of the Space Pirates. Samus escapes during a self-destruct sequence and follows Ridley to the planet Zebes. She searches the planet for the Metroid and finds that the Pirates have rebuilt their base there.",
					}
				},
				ui::Element {
					.style = {
						.margin = {
							.bottom = 16,
						},
					},
					.content = ui::Text {
						.text = "After defeating three bosses in various regions of Zebes, Samus confronts Ridley in his lair and defeats him, only to discover that the capsule containing the Metroid larva has been shattered and the larva is missing. She then heads for Tourian, the heart of the Space Pirates' base, and fights several Metroids that have reproduced. Samus confronts the Metroid larva, which has grown to enormous size. It attacks and nearly kills Samus, but relents at the last moment. As Samus was present at its hatching on SR388, the Metroid has imprinted on Samus, and recognizes her as its \"mother\".",
					}
				},
			}
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}
