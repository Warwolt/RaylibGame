#include "game/ui/style.h"

#include <gtest/gtest.h>
#include <raylib.h>

ui::Style style_with_overrides() {
	return ui::Style {
		.border = {
			.color = RED,
			.image = ImageID(1),
			.image_slices = { 1, 1, 1, 1 },
			.image_fill_center = false,
		},
		.background = {
			.color = RED,
			.image = ImageID(1),
			.fill = ui::Fill::Stretch,
		},
		.font = {
			.id = FontID(1),
			.size = 1,
			.color = RED,
		},
		.hover = {
			.border = {
				.color = GREEN,
				.image = ImageID(2),
				.image_slices = ui::Edges { 2, 2, 2, 2 },
				.image_fill_center = true,
			},
			.background = {
				.color = GREEN,
				.image = ImageID(2),
				.fill = ui::Fill::Repeat,
			},
			.font = {
				.id = FontID(2),
				.size = 2,
				.color = GREEN,
			},
		},
		.active = {
			.border = {
				.color = BLUE,
				.image = ImageID(3),
				.image_slices = ui::Edges { 3, 3, 3, 3 },
				.image_fill_center = true,
			},
			.background = {
				.color = BLUE,
				.image = ImageID(3),
				.fill = ui::Fill::Repeat,
			},
			.font = {
				.id = FontID(3),
				.size = 3,
				.color = BLUE,
			},
		},
	};
}

TEST(StyleTests, StyleOverride_Inactive_GivesNormalStyle) {
	const ui::Style style = style_with_overrides();
	const ui::Style overriden_style = ui::get_overriden_style(style, ui::StyleState::Inactive);

	EXPECT_EQ(overriden_style.border.color, RED);
	EXPECT_EQ(overriden_style.border.image, ImageID(1));
	EXPECT_EQ(overriden_style.border.image_slices, ui::Edges(1, 1, 1, 1));
	EXPECT_EQ(overriden_style.border.image_fill_center, false);

	EXPECT_EQ(overriden_style.background.color, RED);
	EXPECT_EQ(overriden_style.background.image, ImageID(1));
	EXPECT_EQ(overriden_style.background.fill, ui::Fill::Stretch);

	EXPECT_EQ(overriden_style.font.id, FontID(1));
	EXPECT_EQ(overriden_style.font.size, 1);
	EXPECT_EQ(overriden_style.font.color, RED);
}

TEST(StyleTests, StyleOverride_Hovered_GivesHoverStyle) {
	const ui::Style style = style_with_overrides();
	const ui::Style overriden_style = ui::get_overriden_style(style, ui::StyleState::Hover);

	EXPECT_EQ(overriden_style.border.color, GREEN);
	EXPECT_EQ(overriden_style.border.image, ImageID(2));
	EXPECT_EQ(overriden_style.border.image_slices, ui::Edges(2, 2, 2, 2));
	EXPECT_EQ(overriden_style.border.image_fill_center, true);

	EXPECT_EQ(overriden_style.background.color, GREEN);
	EXPECT_EQ(overriden_style.background.image, ImageID(2));
	EXPECT_EQ(overriden_style.background.fill, ui::Fill::Repeat);

	EXPECT_EQ(overriden_style.font.id, FontID(2));
	EXPECT_EQ(overriden_style.font.size, 2);
	EXPECT_EQ(overriden_style.font.color, GREEN);
}

TEST(StyleTests, StyleOverride_Active_GivesActiveStyle) {
	const ui::Style style = style_with_overrides();
	const ui::Style overriden_style = ui::get_overriden_style(style, ui::StyleState::Active);

	EXPECT_EQ(overriden_style.border.color, BLUE);
	EXPECT_EQ(overriden_style.border.image, ImageID(3));
	EXPECT_EQ(overriden_style.border.image_slices, ui::Edges(3, 3, 3, 3));
	EXPECT_EQ(overriden_style.border.image_fill_center, true);

	EXPECT_EQ(overriden_style.background.color, BLUE);
	EXPECT_EQ(overriden_style.background.image, ImageID(3));
	EXPECT_EQ(overriden_style.background.fill, ui::Fill::Repeat);

	EXPECT_EQ(overriden_style.font.id, FontID(3));
	EXPECT_EQ(overriden_style.font.size, 3);
	EXPECT_EQ(overriden_style.font.color, BLUE);
}
