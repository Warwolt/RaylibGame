#include "game/ui/user_interface.h"

#include "test/snapshot_tests/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

#include <format>

constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;
constexpr Vector2 SCREEN_SIZE = { SCREEN_WIDTH, SCREEN_HEIGHT };
#define LIGHTGREEN Color(191, 240, 172, 255)

class ElementSnapshotTests : public ::testing::Test {
public:
	static ResourceManager m_resources;
	static ImageID m_big_test_image;
	static ImageID m_small_test_image;
	static ImageID m_nine_slice_image;

	static void SetUpTestSuite() {
		Raylib_SetTraceLogLevel(LOG_WARNING);
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN);
		Raylib_InitWindow(1, 1, "Unit Test");
		m_resources.load_default_font("resource/font/8bitoperator_jve.ttf");
		m_big_test_image = m_resources.load_image("resource/image/test/utah_teapot_1000_818.png").value();
		m_small_test_image = m_resources.load_image("resource/image/test/chess_board_145_145.png").value();
		m_nine_slice_image = m_resources.load_image("resource/image/test/nine_slice_48_48.png").value();
	}

	static void TearDownTestSuite() {
		m_resources = {};
		Raylib_CloseWindow();
	}
};

ResourceManager ElementSnapshotTests::m_resources;
ImageID ElementSnapshotTests::m_big_test_image;
ImageID ElementSnapshotTests::m_small_test_image;
ImageID ElementSnapshotTests::m_nine_slice_image;

TEST_F(ElementSnapshotTests, Box_100_100_Gives_50_50) {
	ui::Context context = {};
	ui::Element element = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.background = { .color = RED },
					},
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.background = { .color = GREEN },
					},
				}
			}
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.box()->children[0].layout.margin_box.width, SCREEN_WIDTH / 2);
	EXPECT_EQ(element.box()->children[1].layout.margin_box.width, SCREEN_WIDTH / 2);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_100_25_100_Gives_37_25_37) {
	ui::Context context = {};
	ui::Element element = {
		.content = ui::Box {
			.direction = ui::Direction::Horizontal,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.background = { .color = RED },
					},
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(25),
						.background = { .color = GREEN },
					},
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.background = { .color = BLUE },
					},
				}
			}
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.box()->children[0].layout.margin_box.width, SCREEN_WIDTH * 0.375);
	EXPECT_EQ(element.box()->children[1].layout.margin_box.width, SCREEN_WIDTH * 0.25);
	EXPECT_EQ(element.box()->children[2].layout.margin_box.width, SCREEN_WIDTH * 0.375);
	EXPECT_SNAPSHOT_EQ(image);
}

ui::Element box_alignment(ui::Alignment alignment, ui::Direction direction) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.alignment = alignment,
			.cross_alignment = alignment,
		},
		.content = ui::Box {
			.direction = direction,
			.children = {},
		},
	};
	for (int i = 0; i < 3; i++) {
		element.box()->children.push_back(
			ui::Element {
				.style = {
					.width = ui::Pixels(100),
					.height= ui::Pixels(100),
					.margin = ui::Edges::uniform(2),
					.border = {
						.edges = ui::Edges::uniform(2),
						.color = DARKBLUE,
					},
					.alignment = ui::Alignment::Center,
					.cross_alignment = ui::Alignment::Center,
					.background = { .color = BLUE },
					.font = { .color = WHITE },
				},
				.content = ui::Text {
					.text = std::format("{}", i + 1),
				},
			}
		);
	}
	return element;
}

TEST_F(ElementSnapshotTests, Box_Alignment_StartStart_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_alignment(ui::Alignment::Start, ui::Direction::Horizontal);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.box()->children[0].layout.content_box.width, 100);
	EXPECT_EQ(element.box()->children[0].layout.content_box.height, 100);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_Alignment_StartStart_Vertical) {
	ui::Context context = {};
	ui::Element element = box_alignment(ui::Alignment::Start, ui::Direction::Vertical);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_Alignment_CenterCenter_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_alignment(ui::Alignment::Center, ui::Direction::Horizontal);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_Alignment_CenterCenter_Vertical) {
	ui::Context context = {};
	ui::Element element = box_alignment(ui::Alignment::Center, ui::Direction::Vertical);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_Alignment_EndEnd_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_alignment(ui::Alignment::End, ui::Direction::Horizontal);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_Alignment_EndEnd_Vertical) {
	ui::Context context = {};
	ui::Element element = box_alignment(ui::Alignment::End, ui::Direction::Vertical);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

ui::Element box_with_position(ui::Direction direction, ui::Position position) {
	return {
		.style = {
			.border = ui::Edges::uniform(2),
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
		},
		.content = ui::Box {
			.direction = direction,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Pixels(100),
						.height= ui::Pixels(100),
						.margin = ui::Edges::uniform(2),
						.border = {
							.edges = ui::Edges::uniform(2),
							.color = DARKBLUE,
						},
						.alignment = ui::Alignment::Center,
						.cross_alignment = ui::Alignment::Center,
						.background = { .color = BLUE },
						.font = { .color = WHITE },
					},
				},
				ui::Element {
					.style = {
						.position = position,
						.width = ui::Pixels(100),
						.height = ui::Pixels(100),
						.margin = ui::Edges::uniform(2),
						.border = {
							.edges = ui::Edges::uniform(2),
							.color = GOLD,
						},
						.alignment = ui::Alignment::Center,
						.cross_alignment = ui::Alignment::Center,
						.background = { .color = YELLOW },
						.font = { .color = WHITE },
					},
				},
				ui::Element {
					.style = {
						.width = ui::Pixels(100),
						.height= ui::Pixels(100),
						.margin = ui::Edges::uniform(2),
						.border = {
							.edges = ui::Edges::uniform(2),
							.color = DARKBLUE,
						},
						.alignment = ui::Alignment::Center,
						.cross_alignment = ui::Alignment::Center,
						.background = { .color = BLUE },
						.font = { .color = WHITE },
					},
				},
			}
		}
	};
}

TEST_F(ElementSnapshotTests, Box_RelativePosition_Pixels_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Horizontal,
		ui::RelativePosition {
			.x = ui::Pixels(-50),
			.y = ui::Pixels(-50),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.box()->children[0].layout.content_box.width, 100);
	EXPECT_EQ(element.box()->children[0].layout.content_box.height, 100);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_RelativePosition_Percentage_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Horizontal,
		ui::RelativePosition {
			.x = ui::Percentage(-50),
			.y = ui::Percentage(-50),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_RelativePosition_Pixels_Vertical) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Vertical,
		ui::RelativePosition {
			.x = ui::Pixels(-50),
			.y = ui::Pixels(-50),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_RelativePosition_Percentage_Vertical) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Vertical,
		ui::RelativePosition {
			.x = ui::Percentage(-50),
			.y = ui::Percentage(-50),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_AbsolutePosition_RelativeRoot_Pixels_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Horizontal,
		ui::AbsolutePosition {
			.x = ui::Pixels(100),
			.y = ui::Pixels(100),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_AbsolutePosition_RelativeRoot_Percentage_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Horizontal,
		ui::AbsolutePosition {
			.x = ui::Percentage(50),
			.y = ui::Percentage(50),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_AbsolutePosition_RelativeRoot_Pixels_Vertical) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Vertical,
		ui::AbsolutePosition {
			.x = ui::Pixels(100),
			.y = ui::Pixels(100),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_AbsolutePosition_RelativeRoot_Percentage_Vertical) {
	ui::Context context = {};
	ui::Element element = box_with_position(
		ui::Direction::Vertical,
		ui::AbsolutePosition {
			.x = ui::Percentage(50),
			.y = ui::Percentage(50),
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_AbsolutePosition_CanOverflowParent_Pixels) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.position = ui::AbsolutePosition {
				.x = ui::Pixels(100),
				.y = ui::Pixels(100),
			},
			.width = ui::Pixels(200),
			.height = ui::Pixels(50),
			.border = {
				.edges = ui::Edges::uniform(2),
				.color = GREEN,
			},
		},
		.content =
			ui::Box {
				.direction = ui::Direction::Horizontal,
				.children = {
					ui::Element {
						.style = {
							.position = ui::AbsolutePosition {
								.x = ui::Pixels(0),
								.y = ui::Pixels(0),
							},
							.width = ui::Pixels(100),
							.height = ui::Pixels(100),
							.border = {
								.edges = ui::Edges::uniform(2),
								.color = ORANGE,
							},
						},
						.content = ui::Box {},
					},
				},
			},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

ui::Element box_fit_content(ui::Direction direction, std::vector<ui::Element> children) {
	return {
		.style = {
			.fit_content = true,
			.border = {
				.edges = ui::Edges::uniform(2),
				.color = GREEN,
			},
		},
		.content =
			ui::Box {
				.direction = direction,
				.children = children,
			}
	};
}

TEST_F(ElementSnapshotTests, Box_FitContent_NoChildren_Empty) {
	ui::Context context = {};
	ui::Element element = box_fit_content(ui::Direction::Horizontal, {});

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_FitContent_TextChild_FitsText) {
	ui::Context context = {};
	ui::Element element = box_fit_content(
		ui::Direction::Horizontal,
		{
			ui::Element {
				.content =
					ui::Text {
						.text = "Hello world",
					},
			},
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_FitContent_TextImageChild_FitsImage_Horizontal) {
	ui::Context context = {};
	ui::Element element = box_fit_content(
		ui::Direction::Horizontal,
		{
			ui::Element {
				.content =
					ui::Text {
						.text = "Hello world",
					},
			},
			ui::Element {
				.content =
					ui::Image {
						.id = m_small_test_image,
					},
			},
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_FitContent_TextImageChild_FitsImage_Vertical) {
	ui::Context context = {};
	ui::Element element = box_fit_content(
		ui::Direction::Vertical,
		{
			ui::Element {
				.content =
					ui::Text {
						.text = "Hello world",
					},
			},
			ui::Element {
				.content =
					ui::Image {
						.id = m_small_test_image,
					},
			},
		}
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

ui::Element button_box() {
	return {
		.id = "button_box",
		.style = {
			.width = ui::Pixels(200),
			.height = ui::Pixels(100),
			.border = {
				.edges = ui::Edges::uniform(10),
				.color = DARKGREEN,
			},
			.padding = ui::Edges::uniform(10),
			.alignment = ui::Alignment::Center,
			.cross_alignment = ui::Alignment::Center,
			.background = {
				.color = GREEN,
			},
			.font = {
				.color = DARKGREEN,
			},

			.hovered = {
				.border = { .color  = GREEN },
				.background = { .color = LIGHTGREEN },
			},

			.active = {
				.border = { .color  = GREEN },
				.background = { .color = DARKGREEN },
				.font_color = GREEN,
			},
		},
		.content = ui::Text { .text = "Press Me" },
	};
}

TEST_F(ElementSnapshotTests, Box_DefaultStyle) {
	ui::Context context = {};
	ui::Element element = button_box();

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.layout.content_box.width, 200);
	EXPECT_EQ(element.layout.content_box.height, 100);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_HoveredStyle) {
	ui::Context context = {};
	ui::Element element = button_box();
	ui::State* state = context.state(element);
	state->is_hovered = true;

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Box_ActiveStyle) {
	ui::Context context = {};
	ui::Element element = button_box();
	ui::State* state = context.state(element);
	state->is_hovered = true;
	state->is_active = true;

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Text_LeftAligned) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
			.padding = ui::Edges::uniform(20),
			.alignment = ui::Alignment::Start,
			.font = { .size = 32 },
		},
		.content = ui::Text {
			.text = "Left Aligned Text",
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Text_CenterAligned) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
			.padding = ui::Edges::uniform(20),
			.alignment = ui::Alignment::Center,
			.font = { .size = 32 },
		},
		.content = ui::Text {
			.text = "Center Aligned Text",
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Text_RightAligned) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
			.padding = ui::Edges::uniform(20),
			.alignment = ui::Alignment::End,
			.font = { .size = 32 },
		},
		.content = ui::Text {
			.text = "Right Aligned Text",
		}
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Text_MultipleParagraphs_WithTitle) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.width = ui::Percentage(100),
			.padding = ui::Edges::uniform(20),
		},
		.content = ui::Box {
			.direction = ui::Direction::Vertical,
			.children = {
				ui::Element {
					.style = {
						.margin = {
							.bottom = 16,
						},
						.font = { .size = 32 },
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
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Image_DefaultStyle_UsesIntrinsicSize) {
	ui::Context context = {};
	ui::Element element = {
		.content =
			ui::Image {
				.id = m_big_test_image,
			},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.layout.content_box.width, SCREEN_SIZE.x);
	EXPECT_EQ(element.layout.content_box.height, SCREEN_SIZE.y);
	EXPECT_SNAPSHOT_EQ(image);
}

ui::Element image_relative_size(ui::Style parent_style, ui::Direction direction, ImageID image) {
	return {
		.style = parent_style,
		.content = ui::Box {
			.direction = direction,
			.children = {
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.height = ui::Percentage(100),
					},
					.content = ui::Image {
						.id = image,
					}
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.height = ui::Percentage(100),
					},
					.content = ui::Image {
						.id = image,
					}
				},
				ui::Element {
					.style = {
						.width = ui::Percentage(100),
						.height = ui::Percentage(100),
					},
					.content = ui::Image {
						.id = image,
					}
				},
			},
		},
	};
}

TEST_F(ElementSnapshotTests, Image_RelativeSize_FitHorizontally) {
	ui::Context context = {};
	ui::Element element = image_relative_size(
		ui::Style {
			.width = ui::Percentage(100),
			.height = ui::Percentage(50),
		},
		ui::Direction::Horizontal,
		m_big_test_image
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Image_RelativeSize_FitVertically) {
	ui::Context context = {};
	ui::Element element = image_relative_size(
		ui::Style {
			.width = ui::Percentage(50),
			.height = ui::Percentage(100),
		},
		ui::Direction::Vertical,
		m_big_test_image
	);

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Image_PixelSize_FitsInContainer) {
	ui::Context context = {};
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
								.id = m_big_test_image,
							},
					},
					ui::Element {
					.style = ui::Style {
						.width = ui::Pixels(128),
						.height = ui::Pixels(100),
					},
					.content =
						ui::Image {
							.id = m_big_test_image,
						},
					},
					ui::Element {
					.style = ui::Style {
						.width = ui::Pixels(256),
						.height = ui::Pixels(200),
					},
					.content =
						ui::Image {
							.id = m_big_test_image,
						},
					},
				},
			},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.box()->children[0].layout.content_box.width, 64);
	EXPECT_EQ(element.box()->children[0].layout.content_box.height, 50);
	EXPECT_EQ(element.box()->children[1].layout.content_box.width, 128);
	EXPECT_EQ(element.box()->children[1].layout.content_box.height, 100);
	EXPECT_EQ(element.box()->children[2].layout.content_box.width, 256);
	EXPECT_EQ(element.box()->children[2].layout.content_box.height, 200);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Image_PixelSize_HorizontalOverflow_GetsClipped) {
	Texture2D texture = m_resources.get_image(m_big_test_image);
	ASSERT_GT(texture.width, SCREEN_SIZE.x) << "Test image is too small for test to make sense!";
	ASSERT_GT(texture.height, SCREEN_SIZE.y) << "Test image is too small for test to make sense!";

	ui::Context context = {};
	ui::Element element = {
		.style = {
			.width = ui::Percentage(50),
			.border = {
				.edges = ui::Edges::uniform(2),
				.color = GREEN,
			},
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
							.id = m_big_test_image,
						},
				},
			},
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_EQ(element.layout.content_box.width, SCREEN_SIZE.x / 2);
	EXPECT_EQ(element.layout.margin_box.width, SCREEN_SIZE.x / 2 + 2 + 2);
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, Image_PixelSize_VerticalOverflow_GetsClipped) {
	Texture2D texture = m_resources.get_image(m_big_test_image);
	ASSERT_GT(texture.width, SCREEN_SIZE.x) << "Test image is too small for test to make sense!";
	ASSERT_GT(texture.height, SCREEN_SIZE.y) << "Test image is too small for test to make sense!";

	ui::Context context = {};
	ui::Element element = {
		.style = {
			.height = ui::Percentage(50),
			.border = {
				.edges = ui::Edges::uniform(2),
				.color = GREEN,
			},
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
							.id = m_big_test_image,
						},
				},
			},
		},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, BackgroundImage_FillByRepeat) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.background = {
				.image = m_small_test_image,
				.fill = ui::Fill::Repeat,
			},
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, BackgroundImage_FillByStretch) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.background = {
				.image = m_small_test_image,
				.fill = ui::Fill::Stretch,
			},
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, BorderImage_NineSlice_FillCenter) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.border = {
				.edges = ui::Edges::uniform(64),
				.image = m_nine_slice_image,
				.image_slices = ui::Edges::uniform(16),
				.image_fill_center = true,
			},
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(ElementSnapshotTests, BorderImage_NineSlice_WithoutCenter) {
	ui::Context context = {};
	ui::Element element = {
		.style = {
			.border = {
				.edges = ui::Edges::uniform(64),
				.image = m_nine_slice_image,
				.image_slices = ui::Edges::uniform(16),
				.image_fill_center = false,
			},
		},
		.content = ui::Box {},
	};

	ui::layout_element(m_resources, SCREEN_SIZE, &element);
	Image image = snapshots::render_image(SCREEN_SIZE, [&]() { ui::draw_element(m_resources, context, element); });

	EXPECT_SNAPSHOT_EQ(image);
}
