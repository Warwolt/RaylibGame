#include "game/scene/main_menu.h"

#include "game/game.h"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <variant>
#include <vector>

namespace ui {

	/* Content */
	struct Element;

	enum class Direction {
		Horizontal,
		Vertical,
	};

	struct TextContent {
		std::string text;
		std::vector<std::string> lines; // computed during layout
	};

	struct BoxContent {
		Direction direction = Direction::Vertical;
		std::vector<Element> children;
	};

	using Content = std::variant<BoxContent, TextContent>;

	/* Style */
	enum class Alignment {
		Start,
		Center,
		End,
	};

	struct Absolute {
		int pixels;
	};

	struct Relative {
		int percentage;
	};

	struct Size {
		std::variant<Absolute, Relative> value;

		Size(Absolute value)
			: value(value) {
		}

		Size(Relative value)
			: value(value) {
		}

		float fit_to_parent(float parent_size) const {
			float pixels = 0.0f;
			if (const Absolute* absolute_width = std::get_if<Absolute>(&this->value)) {
				pixels = std::min<float>(absolute_width->pixels, parent_size);
			}
			if (const Relative* relative_width = std::get_if<Relative>(&this->value)) {
				pixels = (relative_width->percentage / 100.0f) * parent_size;
			}
			return pixels;
		}
	};

	struct Spacing {
		float top;
		float bottom;
		float left;
		float right;

		float horizontal() const {
			return this->left + this->right;
		}

		float vertical() const {
			return this->top + this->bottom;
		}

		static Spacing with_size(float size) {
			return { size, size, size, size };
		}
	};

	struct Style {
		Size width = Relative(100);
		Size height = Relative(100);
		Spacing margin;
		Spacing border;
		Spacing padding;
		Alignment alignment;
		Color border_color = { 0, 0, 0, 0 };
		Color background_color = { 0, 0, 0, 0 };
		Color font_color = BLACK;
		FontID font_id;
		int font_size;

		float horizontal_spacing() const {
			return margin.horizontal() + border.horizontal() + padding.horizontal();
		}

		float vertical_spacing() const {
			return margin.vertical() + border.vertical() + padding.vertical();
		}
	};

	/* Layout */
	struct Layout {
		Rectangle margin_box;
		Rectangle border_box;
		Rectangle padding_box;
		Rectangle content_box;
	};

	/* Element */
	struct Element {
		Style style;
		Content content;
		Layout layout;
	};

	std::vector<std::string> split_words(const std::string& text) {
		std::vector<std::string> words;

		std::istringstream iss(text);
		std::string word;
		while (iss >> word) {
			words.push_back(word);
		}

		return words;
	}

	void compute_element_sizes(const ResourceManager& resources, Vector2 available_size, Element* element) {
		const Style& style = element->style;
		Layout* layout = &element->layout;

		if (TextContent* text_content = std::get_if<TextContent>(&element->content)) {
			const Font& font = resources.get_font(style.font_id);
			const Vector2 max_text_size = {
				.x = style.width.fit_to_parent(available_size.x) - style.horizontal_spacing(),
				.y = style.height.fit_to_parent(available_size.y) - style.vertical_spacing(),
			};
			const int space_width = Raylib_MeasureTextEx(font, " ", style.font_size, 0.0f).x;
			const std::vector<std::string> words = split_words(text_content->text);

			// FIXME: review this text size code
			// I had to let a robot help me fix some of the logic here, so we
			// should sit down and think through what should really be going on
			// in this computation.
			Vector2 cursor = { 0, 0 };
			text_content->lines.push_back("");
			for (const std::string& word : words) {
				const int word_length = Raylib_MeasureTextEx(font, word.c_str(), style.font_size, 0.0f).x;
				const float projected_x = cursor.x > 0 ? cursor.x + space_width + word_length : word_length;
				if (projected_x <= max_text_size.x) {
					// word fits on the current line
					if (cursor.x > 0) {
						text_content->lines.back() += " ";
						cursor.x += space_width;
					}
					text_content->lines.back() += word;
					cursor.x += word_length;
				} else {
					// switch to new line
					cursor.x = 0;
					cursor.y = cursor.y + style.font_size;
					if (cursor.y + style.font_size > max_text_size.y) {
						break;
					}
					text_content->lines.push_back("");
					text_content->lines.back() += word;
					cursor.x = word_length;
				}
			}
			const int text_height = std::min<int>(cursor.y + style.font_size, max_text_size.y);

			layout->content_box.width = max_text_size.x;
			layout->content_box.height = text_height;
		}

		if (BoxContent* box_content = std::get_if<BoxContent>(&element->content)) {
			/* Size content box */
			const float element_width = style.width.fit_to_parent(available_size.x);
			const float element_height = style.height.fit_to_parent(available_size.y);
			layout->content_box.width = element_width - style.horizontal_spacing();
			layout->content_box.height = element_height - style.vertical_spacing();

			/* Size all children */
			for (Element& child : box_content->children) {
				// FIXME: Check what the _desired_ size of the child is, and set
				// the available size to the minimum of desired size and (parent
				// size)/N where N is num children
				Vector2 available_child_size = {};
				if (box_content->direction == Direction::Horizontal) {
					available_child_size = {
						.x = layout->content_box.width / box_content->children.size(),
						.y = layout->content_box.height,
					};
				}
				if (box_content->direction == Direction::Vertical) {
					available_child_size = {
						.x = layout->content_box.width,
						.y = layout->content_box.height / box_content->children.size(),
					};
				}
				compute_element_sizes(resources, available_child_size, &child);
			}
		}

		layout->padding_box.width = layout->content_box.width + style.padding.left + style.padding.right;
		layout->padding_box.height = layout->content_box.height + style.padding.top + style.padding.bottom;
		layout->border_box.width = layout->padding_box.width + style.border.left + style.border.right;
		layout->border_box.height = layout->padding_box.height + style.border.top + style.border.bottom;
		layout->margin_box.width = layout->border_box.width + style.margin.left + style.margin.right;
		layout->margin_box.height = layout->border_box.height + style.margin.top + style.margin.bottom;
	}

	void compute_element_positions(Vector2 position, Element* element) {
		const Style style = element->style;
		Layout* layout = &element->layout;

		/* Position all boxes relative to each other */
		layout->margin_box.x = position.x;
		layout->margin_box.y = position.y;
		layout->border_box.x = layout->margin_box.x + style.margin.left;
		layout->border_box.y = layout->margin_box.y + style.margin.top;
		layout->padding_box.x = layout->border_box.x + style.border.left;
		layout->padding_box.y = layout->border_box.y + style.border.top;
		layout->content_box.x = layout->padding_box.x + style.padding.left;
		layout->content_box.y = layout->padding_box.y + style.padding.top;

		/* Recurse into children */
		if (BoxContent* box_content = std::get_if<BoxContent>(&element->content)) {
			Vector2 cursor = { layout->content_box.x, layout->content_box.y };
			for (Element& child : box_content->children) {
				/* Compute child position */
				Vector2 element_size = { layout->margin_box.width, layout->margin_box.height };
				Vector2 child_position = cursor;
				compute_element_positions(child_position, &child);

				/* Move cursor */
				switch (box_content->direction) {
					case Direction::Horizontal:
						cursor.x += child.layout.margin_box.width;
						break;
					case Direction::Vertical:
						cursor.y += child.layout.margin_box.height;
						break;
				}
			}
		}
	}

	void compute_element_layout(const ResourceManager& resources, Vector2 window_size, Element* element) {
		const Vector2 top_left = { 0, 0 };
		compute_element_sizes(resources, window_size, element);
		compute_element_positions(top_left, element);
	}

	void draw_element(const ResourceManager& resources, const Element& element) {
		const Style& style = element.style;
		Raylib_DrawRectangleRec(element.layout.border_box, element.style.border_color);
		Raylib_DrawRectangleRec(element.layout.padding_box, element.style.background_color);

		const bool show_debug_outline = true;
		if (show_debug_outline) {
			Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, GREEN);
		}

		if (const ui::TextContent* text_content = std::get_if<ui::TextContent>(&element.content)) {
			const Font& font = resources.get_font(style.font_id);
			const Rectangle content_box = element.layout.content_box;

			int line_num = 0;
			for (const std::string& line : text_content->lines) {
				const int line_length = Raylib_MeasureTextEx(font, line.c_str(), style.font_size, 0.0f).x;
				const int remainder = content_box.width - line_length;
				int left_padding = 0;
				switch (style.alignment) {
					case Alignment::Start:
						left_padding = 0;
						break;
					case Alignment::Center:
						left_padding = remainder / 2;
						break;
					case Alignment::End:
						left_padding = remainder;
						break;
				}
				Vector2 line_pos = {
					.x = element.layout.content_box.x + left_padding,
					.y = element.layout.content_box.y + line_num * style.font_size,
				};

				Raylib_BeginScissorMode(content_box.x, content_box.y, content_box.width, content_box.height);
				{
					Raylib_DrawTextEx(font, line.c_str(), line_pos, style.font_size, 0.0f, style.font_color);
				}
				Raylib_EndScissorMode();
				line_num++;
			}
		}
		if (const ui::BoxContent* box_content = std::get_if<ui::BoxContent>(&element.content)) {
			for (const Element& child : box_content->children) {
				draw_element(resources, child);
			}
		}
	}

	void debug_draw_element_boxes(const Layout& element_boxes) {
		const Color margin_color = { 240, 206, 163, 255 };
		const Color border_color = { 246, 221, 161, 255 };
		const Color padding_color = { 198, 207, 145, 255 };
		const Color content_color = { 152, 181, 192, 255 };
		Raylib_DrawRectangleRec(element_boxes.margin_box, margin_color);
		Raylib_DrawRectangleRec(element_boxes.border_box, border_color);
		Raylib_DrawRectangleRec(element_boxes.padding_box, padding_color);
		Raylib_DrawRectangleRec(element_boxes.content_box, content_color);
	}
}

void MainMenuScene::initialize(Game* /*game*/) {
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
}

void MainMenuScene::render(const Game& game) const {
	Raylib_ClearBackground(BLUE);

	/* Input */
	ui::Style text_style = {
		.margin = ui::Spacing::with_size(7),
		.border = ui::Spacing::with_size(0),
		.padding = ui::Spacing::with_size(0),
		.alignment = ui::Alignment::Start,
		.border_color = { 0 },
		.background_color = { 0 },
		.font_color = WHITE,
		.font_id = FontID::default_font(),
		.font_size = 16,
	};
	ui::Element root_element = {
		.style = {
			.width = ui::Relative(50),
			.height = ui::Relative(100),
		},
		.content =
			ui::BoxContent {
				.direction = ui::Direction::Vertical,
				.children = {
					ui::Element {
						.style = text_style,
						.content =
							ui::TextContent {
								.text = "Samus Aran brings the last Metroid to the Ceres space colony for scientific study. Investigation of the specimen, a larva, reveals that its energy-producing abilities could be harnessed for the good of civilization. Shortly after leaving, Samus receives a distress call alerting her to return to the colony immediately. She finds the scientists dead, and the Metroid larva stolen by Ridley, leader of the Space Pirates. Samus escapes during a self-destruct sequence and follows Ridley to the planet Zebes.[12] She searches the planet for the Metroid and finds that the Pirates have rebuilt their base there.[4]: 5 ",
							},
					},
					ui::Element {
						.style = text_style,
						.content =
							ui::TextContent {
								.text = "After defeating three bosses in various regions of Zebes, Samus confronts Ridley in his lair and defeats him, only to discover that the capsule containing the Metroid larva has been shattered and the larva is missing. She then heads for Tourian,[5]: 109  the heart of the Space Pirates' base, and fights several Metroids that have reproduced. Samus confronts the Metroid larva, which has grown to enormous size. It attacks and nearly kills Samus, but relents at the last moment. As Samus was present at its hatching on SR388, the Metroid has imprinted on Samus, and recognizes her as its \"mother\".[5]: 113 [7][13]",
							},
					},
				},
			},
	};

	/* Compute layout */
	ui::compute_element_layout(game.resources, game.window.size(), &root_element);

	/* Render elements */
	ui::draw_element(game.resources, root_element);
}
