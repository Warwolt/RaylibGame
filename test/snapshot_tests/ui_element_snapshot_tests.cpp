#include "game/ui.h"

#include "test/snapshot_tests/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <format>

constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;
constexpr Vector2 SCREEN_SIZE = { SCREEN_WIDTH, SCREEN_HEIGHT };
#define LIGHTGREEN Color(191, 240, 172, 255)

const ui::Style button_style = {
		.width = ui::Pixels(200),
		.height = ui::Pixels(100),
		.border = ui::Spacing::uniform(10),
		.padding = ui::Spacing::uniform(10),
		.alignment = ui::Alignment::Center,
		.cross_alignment = ui::Alignment::Center,
		.background_color = GREEN,
		.font_color = DARKGREEN,
		.border_color = DARKGREEN,

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

class UIElementSnapshotTests : public ::testing::Test {
public:
	ResourceManager m_resources;
	ImageID m_big_test_image;
	ImageID m_small_test_image;
	ImageID m_nine_slice_image;

	void SetUp() {
		Raylib_SetTraceLogLevel(LOG_WARNING);
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN);
		Raylib_InitWindow(1, 1, "Unit Test");
		m_resources.load_default_font("resource/font/ModernDOS8x16.ttf");
		m_big_test_image = m_resources.load_image("resource/image/test/utah_teapot_1000_818.png").value();
		m_small_test_image = m_resources.load_image("resource/image/test/chess_board_145_145.png").value();
		m_nine_slice_image = m_resources.load_image("resource/image/test/nine_slice_48_48.png").value();
	}

	void TearDown() {
		Raylib_CloseWindow();
	}
};

TEST_F(UIElementSnapshotTests, Box_100_100_Gives_50_50) {
	ui::Element element = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.background_color = RED,
					},
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
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

TEST_F(UIElementSnapshotTests, Box_100_25_100_Gives_37_25_37) {
	ui::Element element = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.background_color = RED,
					},
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(25),
						.background_color = GREEN,
					},
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
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

TEST_F(UIElementSnapshotTests, Box_Alignment_StartStart) {
	ui::Element element = {
		.style = {
			.alignment = ui::Alignment::Start,
			.cross_alignment = ui::Alignment::Start,
		},
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {},
		},
	};
	for (int i = 0; i < 3; i++) {
		element.box()->children.push_back(
			ui::Element {
				.style = {
					.width = ui::Pixels(100),
					.height= ui::Pixels(100),
					.margin = ui::Spacing::uniform(2),
					.border = ui::Spacing::uniform(2),
					.alignment = ui::Alignment::Center,
					.cross_alignment = ui::Alignment::Center,
					.background_color = BLUE,
					.font_color = WHITE,
					.border_color = DARKBLUE,
				},
				.content = ui::Text {
					.text = std::format("Start {}", i + 1),
				},
			}
		);
	}

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}
TEST_F(UIElementSnapshotTests, Box_Alignment_CenterCenter) {
	ui::Element element = {
		.style = {
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
		},
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {},
		},
	};
	for (int i = 0; i < 3; i++) {
		element.box()->children.push_back(
			ui::Element {
				.style = {
					.width = ui::Pixels(100),
					.height= ui::Pixels(100),
					.margin = ui::Spacing::uniform(2),
					.border = ui::Spacing::uniform(2),
					.alignment = ui::Alignment::Center,
					.cross_alignment = ui::Alignment::Center,
					.background_color = BLUE,
					.font_color = WHITE,
					.border_color = DARKBLUE,
				},
				.content = ui::Text {
					.text = std::format("Center {}", i + 1),
				},
			}
		);
	}

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Box_Alignment_EndEnd) {
	ui::Element element = {
		.style = {
			.alignment = ui::Alignment::End,
			.cross_alignment = ui::Alignment::End,
		},
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {},
		},
	};
	for (int i = 0; i < 3; i++) {
		element.box()->children.push_back(
			ui::Element {
				.style = {
					.width = ui::Pixels(100),
					.height= ui::Pixels(100),
					.margin = ui::Spacing::uniform(2),
					.border = ui::Spacing::uniform(2),
					.alignment = ui::Alignment::Center,
					.cross_alignment = ui::Alignment::Center,
					.background_color = BLUE,
					.font_color = WHITE,
					.border_color = DARKBLUE,
				},
				.content = ui::Text {
					.text = std::format("End {}", i + 1),
				},
			}
		);
	}

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

// FIXME: use parameterized tests to get better coverage
// e.g. should test alignment with both vertical and horizontal directions on the containing box
TEST_F(UIElementSnapshotTests, Box_RelativePosition) {
	ui::Element element = {
		.style = {
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
		},
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Pixels(100),
						.height= ui::Pixels(100),
						.margin = ui::Spacing::uniform(2),
						.border = ui::Spacing::uniform(2),
						.alignment = ui::Alignment::Center,
						.cross_alignment = ui::Alignment::Center,
						.background_color = BLUE,
						.font_color = WHITE,
						.border_color = DARKBLUE,
					},
				},
				ui::Element {
					.style = {
						.position = ui::RelativePosition {
							.x = ui::Percentage(-50),
							.y = ui::Percentage(-50),
						},
						.width = ui::Pixels(100),
						.height = ui::Pixels(100),
						.margin = ui::Spacing::uniform(2),
						.border = ui::Spacing::uniform(2),
						.alignment = ui::Alignment::Center,
						.cross_alignment = ui::Alignment::Center,
						.background_color = YELLOW,
						.font_color = WHITE,
						.border_color = GOLD,
					},
				},
				ui::Element {
					.style = {
						.width = ui::Pixels(100),
						.height= ui::Pixels(100),
						.margin = ui::Spacing::uniform(2),
						.border = ui::Spacing::uniform(2),
						.alignment = ui::Alignment::Center,
						.cross_alignment = ui::Alignment::Center,
						.background_color = BLUE,
						.font_color = WHITE,
						.border_color = DARKBLUE,
					},
				},
			}
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Box_DefaultStyle) {
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

TEST_F(UIElementSnapshotTests, Box_HoveredStyle) {
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

TEST_F(UIElementSnapshotTests, Box_ActiveStyle) {
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

TEST_F(UIElementSnapshotTests, Text_LeftAligned) {
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
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

TEST_F(UIElementSnapshotTests, Text_CenterAligned) {
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
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

TEST_F(UIElementSnapshotTests, Text_RightAligned) {
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
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

TEST_F(UIElementSnapshotTests, Text_MultipleParagraphs_WithTitle) {
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
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

TEST_F(UIElementSnapshotTests, Image_DefaultStyle_FillsParentContainer) {
	ui::Element element = {
		.content =
			ui::Image {
				.image = m_big_test_image,
			},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_EQ(element.layout.content_box.width, SCREEN_SIZE.x);
	EXPECT_EQ(element.layout.content_box.height, SCREEN_SIZE.y);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Image_RelativeSize_FitHorizontally) {
	ui::Element element = {
		.style = {
			.height = ui::Percentage(50)
		},
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.content = ui::Image {
						.image = m_big_test_image,
					}
				},
				ui::Element {
					.content = ui::Image {
						.image = m_big_test_image,
					}
				},
				ui::Element {
					.content = ui::Image {
						.image = m_big_test_image,
					}
				},
			},
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Image_RelativeSize_FitVertically) {
	ui::Element element = {
		.style = {
			.width = ui::Percentage(50)
		},
		.content = ui::Box {
			.direction = ui::Direction::Vertical,
			.children = {
				ui::Element {
					.content = ui::Image {
						.image = m_big_test_image,
					}
				},
				ui::Element {
					.content = ui::Image {
						.image = m_big_test_image,
					}
				},
				ui::Element {
					.content = ui::Image {
						.image = m_big_test_image,
					}
				},
			},
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Image_PixelSize_FitsInContainer) {
	ui::Element element = {
		.content =
			ui::Box {
				.direction = ui::Direction::Vertical,
				.children = {
						ui::Element {
						.style = ui::Style {
							.width = ui::Pixels(64),
							.height = ui::Pixels(50),
						},
						.content =
							ui::Image {
								.image = m_big_test_image,
							},
					},
					ui::Element {
					.style = ui::Style {
						.width = ui::Pixels(128),
						.height = ui::Pixels(100),
					},
					.content =
						ui::Image {
							.image = m_big_test_image,
						},
					},
					ui::Element {
					.style = ui::Style {
						.width = ui::Pixels(256),
						.height = ui::Pixels(200),
					},
					.content =
						ui::Image {
							.image = m_big_test_image,
						},
					},
				},
			},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Image_PixelSize_HorizontalOverflow_GetsClipped) {
	Texture2D texture = m_resources.get_image(m_big_test_image);
	ASSERT_GT(texture.width, SCREEN_SIZE.x) << "Test image is too small for test to make sense!";
	ASSERT_GT(texture.height, SCREEN_SIZE.y) << "Test image is too small for test to make sense!";

	ui::Element element = {
		.style = {
			.width = ui::Percentage(50),
			.border = ui::Spacing::uniform(2),
			.border_color = GREEN,
		},
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Pixels(texture.width / 2),
						.height = ui::Pixels(texture.height / 2),
					},
					.content =
						ui::Image {
							.image = m_big_test_image,
						},
				},
			},
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, Image_PixelSize_VerticalOverflow_GetsClipped) {
	Texture2D texture = m_resources.get_image(m_big_test_image);
	ASSERT_GT(texture.width, SCREEN_SIZE.x) << "Test image is too small for test to make sense!";
	ASSERT_GT(texture.height, SCREEN_SIZE.y) << "Test image is too small for test to make sense!";

	ui::Element element = {
		.style = {
			.height = ui::Percentage(50),
			.border = ui::Spacing::uniform(2),
			.border_color = GREEN,
		},
		.content = ui::Box {
			.direction = ui::Direction::Vertical,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Pixels(texture.width / 2),
						.height = ui::Pixels(texture.height / 2),
					},
					.content =
						ui::Image {
							.image = m_big_test_image,
						},
				},
			},
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, BackgroundImage_FillByRepeat) {
	ui::Element element = {
		.style = {
			.background_image = m_small_test_image,
			.background_fill = ui::Fill::Repeat,
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, BackgroundImage_FillByStretch) {
	ui::Element element = {
		.style = {
			.background_image = m_small_test_image,
			.background_fill = ui::Fill::Stretch,
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, BorderImage_NineSlice_FillCenter) {
	ui::Element element = {
		.style = {
			.border = ui::Spacing::uniform(64),
			.border_image = m_nine_slice_image,
			.border_image_slicing = ui::Spacing::uniform(16),
			.border_image_fill_center = true,
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(UIElementSnapshotTests, BorderImage_NineSlice_WithoutCenter) {
	ui::Element element = {
		.style = {
			.border = ui::Spacing::uniform(64),
			.border_image = m_nine_slice_image,
			.border_image_slicing = ui::Spacing::uniform(16),
			.border_image_fill_center = false,
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, element); });

	EXPECT_SNAPSHOT_EQ(image);
}
