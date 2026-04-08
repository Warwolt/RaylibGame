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

	struct AbsoluteSize {
		int pixels;
	};

	struct RelativeSize {
		int percentage; // relative parent size
	};

	struct Size {
		std::variant<AbsoluteSize, RelativeSize> value;

		Size(AbsoluteSize value)
			: value(value) {
		}

		Size(RelativeSize value)
			: value(value) {
		}

		float fit_to_parent(float parent_size) const {
			float pixels = 0.0f;
			if (const AbsoluteSize* absolute_width = std::get_if<AbsoluteSize>(&this->value)) {
				pixels = std::min<float>(absolute_width->pixels, parent_size);
			}
			if (const RelativeSize* relative_width = std::get_if<RelativeSize>(&this->value)) {
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

		static Spacing uniform(float size) {
			return { size, size, size, size };
		}
	};

	struct Style {
		Size width = RelativeSize(100);
		Size height = RelativeSize(100);
		Spacing margin;
		Spacing border;
		Spacing padding;
		Alignment alignment;
		Color border_color = { 0, 0, 0, 0 };
		Color background_color = { 0, 0, 0, 0 };
		Color font_color = WHITE;
		FontID font_id = FontID::default_font();
		int font_size = 16;

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

	float alignment_padding(Alignment alignment, float remainder) {
		switch (alignment) {
			case Alignment::Start:
				return 0;
				break;
			case Alignment::Center:
				return remainder / 2;
				break;
			case Alignment::End:
				return remainder;
				break;
		}
		return 0;
	}

	std::vector<std::string> split_text_into_words(const std::string& text) {
		std::vector<std::string> words;

		std::istringstream iss(text);
		std::string word;
		while (iss >> word) {
			words.push_back(word);
		}

		return words;
	}

	void compute_child_element_sizes(const ResourceManager& resources, Vector2 element_size, Element* element) {
		const Style& style = element->style;
		Layout* layout = &element->layout;

		if (TextContent* text_content = std::get_if<TextContent>(&element->content)) {
			const Font& font = resources.get_font(style.font_id);
			const Vector2 max_text_size = {
				.x = element_size.x - style.horizontal_spacing(),
				.y = element_size.y - style.vertical_spacing(),
			};
			const int space_width = Raylib_MeasureTextEx(font, " ", style.font_size, 0.0f).x;
			/* Fit text to element size */
			Vector2 cursor = { 0, 0 };
			text_content->lines.push_back("");
			for (const std::string& word : split_text_into_words(text_content->text)) {
				const int word_length = Raylib_MeasureTextEx(font, word.c_str(), style.font_size, 0.0f).x;
				const int needed_length = cursor.x > 0 ? space_width + word_length : word_length;
				if (cursor.x + needed_length <= max_text_size.x) {
					// add word to current line
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
			layout->content_box.width = max_text_size.x;
			layout->content_box.height = max_text_size.y;
		}

		if (BoxContent* box_content = std::get_if<BoxContent>(&element->content)) {
			/* Size content box */
			Rectangle& content_box = layout->content_box;
			content_box.width = element_size.x - style.horizontal_spacing();
			content_box.height = element_size.y - style.vertical_spacing();

			/* Size all children */
			{
				// 1. compute desired size of each child
				struct IndexedVector2 {
					size_t index;
					Vector2 value;
				};
				std::vector<IndexedVector2> desired_sizes;
				for (size_t i = 0; i < box_content->children.size(); i++) {
					Element& child = box_content->children[i];
					const Vector2 desired_size = {
						.x = child.style.width.fit_to_parent(content_box.width),
						.y = child.style.height.fit_to_parent(content_box.height),
					};
					desired_sizes.push_back({ i, desired_size });
				}
				// 2. sort desired sizes from smallest to biggest
				auto ordering = [&](const IndexedVector2& lhs, const IndexedVector2& rhs) {
					if (box_content->direction == Direction::Horizontal) {
						return lhs.value.x < rhs.value.x;
					} else {
						return lhs.value.y < rhs.value.y;
					}
				};
				std::sort(desired_sizes.begin(), desired_sizes.end(), ordering);
				// 3. from smallest to biggest, compute actual sizes
				float remaining_width = content_box.width;
				float remaining_height = content_box.height;
				for (size_t i = 0; i < box_content->children.size(); i++) {
					const size_t remaining_children = box_content->children.size() - i;
					const IndexedVector2& desired_size = desired_sizes[i];
					Element& child = box_content->children[desired_size.index];
					if (box_content->direction == Direction::Horizontal) {
						const Vector2 child_size = {
							.x = std::min<float>(desired_size.value.x, remaining_width / remaining_children),
							.y = content_box.height,
						};
						remaining_width -= child_size.x;
						compute_child_element_sizes(resources, child_size, &child);
					} else {
						const Vector2 child_size = {
							.x = content_box.width,
							.y = std::min<float>(desired_size.value.y, remaining_height / remaining_children),
						};
						remaining_height -= child_size.y;
						compute_child_element_sizes(resources, child_size, &child);
					}
				}
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
			/* Compute padding for alignment */
			int left_padding = 0;
			int top_padding = 0;
			switch (box_content->direction) {
				case Direction::Horizontal: {
					int total_element_widths = 0;
					for (Element& child : box_content->children) {
						total_element_widths += child.layout.margin_box.width;
					}
					const int horizontal_remainder = element->layout.content_box.width - total_element_widths;
					left_padding = alignment_padding(element->style.alignment, horizontal_remainder);
				} break;
				case Direction::Vertical: {
					int total_element_heights = 0;
					for (Element& child : box_content->children) {
						total_element_heights += child.layout.margin_box.height;
					}
					const int vertical_remainder = element->layout.content_box.height - total_element_heights;
					top_padding = alignment_padding(element->style.alignment, vertical_remainder);
				} break;
			}

			Vector2 cursor = {
				.x = layout->content_box.x + left_padding,
				.y = layout->content_box.y + top_padding,
			};
			for (Element& child : box_content->children) {
				/* Compute child position */
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
		compute_child_element_sizes(resources, window_size, element);
		compute_element_positions(top_left, element);
	}

	void draw_element(const ResourceManager& resources, const Element& element) {
		const Style& style = element.style;
		Raylib_DrawRectangleRec(element.layout.padding_box, element.style.background_color);

		/* Draw border */
		{
			const Rectangle border_top = {
				.x = element.layout.border_box.x,
				.y = element.layout.border_box.y,
				.width = element.layout.border_box.width,
				.height = element.style.border.top,
			};
			const Rectangle border_bottom = {
				.x = element.layout.border_box.x,
				.y = element.layout.padding_box.y + element.layout.padding_box.height,
				.width = element.layout.border_box.width,
				.height = element.style.border.bottom,
			};
			const Rectangle border_left = {
				.x = element.layout.border_box.x,
				.y = element.layout.padding_box.y,
				.width = element.style.border.left,
				.height = element.layout.padding_box.height,
			};
			const Rectangle border_right = {
				.x = element.layout.border_box.x + element.layout.border_box.width - element.style.border.right,
				.y = element.layout.padding_box.y,
				.width = element.style.border.right,
				.height = element.layout.padding_box.height,
			};
			Raylib_DrawRectangleRec(border_top, element.style.border_color);
			Raylib_DrawRectangleRec(border_bottom, element.style.border_color);
			Raylib_DrawRectangleRec(border_left, element.style.border_color);
			Raylib_DrawRectangleRec(border_right, element.style.border_color);
		}

		const bool show_debug_outline = false;
		if (show_debug_outline) {
			Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, GREEN);
		}

		if (const ui::TextContent* text_content = std::get_if<ui::TextContent>(&element.content)) {
			const Font& font = resources.get_font(style.font_id);
			const Rectangle content_box = element.layout.content_box;
			Raylib_BeginScissorMode(content_box.x, content_box.y, content_box.width, content_box.height);
			{
				int line_num = 0;
				for (const std::string& line : text_content->lines) {
					const int line_length = Raylib_MeasureTextEx(font, line.c_str(), style.font_size, 0.0f).x;
					const int left_padding = alignment_padding(style.alignment, content_box.width - line_length);
					Vector2 line_pos = {
						.x = element.layout.content_box.x + left_padding,
						.y = element.layout.content_box.y + line_num * style.font_size,
					};
					Raylib_DrawTextEx(font, line.c_str(), line_pos, style.font_size, 0.0f, style.font_color);
					line_num++;
				}
			}
			Raylib_EndScissorMode();
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
	Raylib_ClearBackground(BLACK);

	/* Input */
	ui::Element root_element = {
		.style = {
			.width = ui::RelativeSize(100),
			.height = ui::RelativeSize(100),
			.margin = ui::Spacing::uniform(0),
			.alignment = ui::Alignment::Start,
		},
		.content =
			ui::BoxContent {
				.direction = ui::Direction::Vertical,
				.children = {
					ui::Element {
						.style = {
							.height = ui::RelativeSize(100),
						},
						.content = ui::BoxContent {
							.direction = ui::Direction::Horizontal,
							.children = {
								ui::Element {
									.style = {
										.width = ui::RelativeSize(100),
										.border = ui::Spacing::uniform(5),
										.padding = ui::Spacing::uniform(10),
										.border_color = BLUE,
										.font_color = WHITE,
									},
									.content = ui::TextContent {
										.text = "Samus Aran brings the last Metroid to the Ceres space colony for scientific study. Investigation of the specimen, a larva, reveals that its energy-producing abilities could be harnessed for the good of civilization. Shortly after leaving, Samus receives a distress call alerting her to return to the colony immediately. She finds the scientists dead, and the Metroid larva stolen by Ridley, leader of the Space Pirates. Samus escapes during a self-destruct sequence and follows Ridley to the planet Zebes. She searches the planet for the Metroid and finds that the Pirates have rebuilt their base there.",
									},
								},
								ui::Element {
									.style = {
										.width = ui::RelativeSize(100),
										.border = ui::Spacing::uniform(5),
										.padding = ui::Spacing::uniform(10),
										.border_color = GREEN,
										.font_color = WHITE,
									},
									.content = ui::TextContent {
										.text = "After defeating three bosses in various regions of Zebes, Samus confronts Ridley in his lair and defeats him, only to discover that the capsule containing the Metroid larva has been shattered and the larva is missing. She then heads for Tourian, the heart of the Space Pirates' base, and fights several Metroids that have reproduced. Samus confronts the Metroid larva, which has grown to enormous size. It attacks and nearly kills Samus, but relents at the last moment. As Samus was present at its hatching on SR388, the Metroid has imprinted on Samus, and recognizes her as its \"mother\"."
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
						.content = ui::TextContent {
							.text = "> All systems nominal"
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
