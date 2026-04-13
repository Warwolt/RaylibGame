#include "game/ui.h"

#include "core/util.h"

#include <algorithm>
#include <memory>

namespace ui {

	static float alignment_padding(Alignment alignment, float remainder) {
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

	static float fit_size_to_parent(const Size& size, float parent_size) {
		float pixels = 0.0f;
		if (const AbsoluteSize* absolute_width = std::get_if<AbsoluteSize>(&size)) {
			pixels = std::min<float>(absolute_width->pixels, parent_size);
		}
		if (const RelativeSize* relative_width = std::get_if<RelativeSize>(&size)) {
			pixels = (relative_width->percentage / 100.0f) * parent_size;
		}
		return pixels;
	}

	static void compute_element_sizes(const ResourceManager& resources, Vector2 max_size, Element* element) {
		const Style& style = element->style;
		Layout* layout = &element->layout;

		if (Text* text = std::get_if<Text>(&element->content)) {
			const Font& font = resources.get_font(style.font_id);
			const float font_spacing = 0.0f;
			const float max_text_width = max_size.x - style.horizontal_spacing();
			const float max_text_height = max_size.y - style.vertical_spacing();
			const int space_width = Raylib_MeasureTextEx(font, " ", style.font_size, font_spacing).x;
			/* Fit text to element size */
			Vector2 cursor = { 0, 0 };
			text->lines.push_back("");
			for (const std::string& word : util::split_text_into_words(text->text)) {
				const int word_length = Raylib_MeasureTextEx(font, word.c_str(), style.font_size, font_spacing).x;
				const int needed_length = cursor.x > 0 ? space_width + word_length : word_length;
				if (cursor.x + needed_length <= max_text_width) {
					// add word to current line
					if (cursor.x > 0) {
						text->lines.back() += " ";
						cursor.x += space_width;
					}
					text->lines.back() += word;
					cursor.x += word_length;
				} else {
					// switch to new line
					cursor.x = 0;
					cursor.y = cursor.y + style.font_size;
					if (cursor.y + style.font_size > max_text_height) {
						break;
					}
					text->lines.push_back("");
					text->lines.back() += word;
					cursor.x = word_length;
				}
			}
			layout->content_box.width = max_text_width;
			layout->content_box.height = std::min(max_text_height, cursor.y + style.font_size);
		}

		if (Box* box = std::get_if<Box>(&element->content)) {
			/* Size content box */
			Rectangle& content_box = layout->content_box;
			content_box.width = max_size.x - style.horizontal_spacing();
			content_box.height = max_size.y - style.vertical_spacing();

			/* Size all children */
			{
				// 1. compute desired size of each child
				struct IndexedVector2 {
					size_t index;
					Vector2 value;
				};
				std::vector<IndexedVector2> desired_sizes;
				for (size_t i = 0; i < box->children.size(); i++) {
					Element& child = box->children[i];
					const Vector2 desired_size = {
						.x = fit_size_to_parent(child.style.width, content_box.width),
						.y = fit_size_to_parent(child.style.height, content_box.height),
					};
					desired_sizes.push_back({ i, desired_size });
				}
				// 2. sort desired sizes from smallest to biggest
				auto ordering = [&](const IndexedVector2& lhs, const IndexedVector2& rhs) {
					if (box->direction == Direction::Horizontal) {
						return lhs.value.x < rhs.value.x;
					} else {
						return lhs.value.y < rhs.value.y;
					}
				};
				std::sort(desired_sizes.begin(), desired_sizes.end(), ordering);
				// 3. from smallest to biggest, compute actual sizes
				float remaining_width = content_box.width;
				float remaining_height = content_box.height;
				for (size_t i = 0; i < box->children.size(); i++) {
					const size_t remaining_children = box->children.size() - i;
					const IndexedVector2& desired_size = desired_sizes[i];
					Element& child = box->children[desired_size.index];
					if (box->direction == Direction::Horizontal) {
						const Vector2 child_size = {
							.x = std::min<float>(desired_size.value.x, remaining_width / remaining_children),
							.y = content_box.height,
						};
						remaining_width -= child_size.x;
						compute_element_sizes(resources, child_size, &child);
					} else {
						const Vector2 child_size = {
							.x = content_box.width,
							.y = std::min<float>(desired_size.value.y, remaining_height / remaining_children),
						};
						compute_element_sizes(resources, child_size, &child);
						remaining_height -= child.layout.margin_box.height;
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

	static void compute_element_positions(Vector2 position, Element* element) {
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
		if (Box* box = std::get_if<Box>(&element->content)) {
			/* Compute padding for alignment */
			int left_padding = 0;
			int top_padding = 0;
			switch (box->direction) {
				case Direction::Horizontal: {
					int total_element_widths = 0;
					for (Element& child : box->children) {
						total_element_widths += child.layout.margin_box.width;
					}
					const int horizontal_remainder = element->layout.content_box.width - total_element_widths;
					left_padding = alignment_padding(element->style.alignment, horizontal_remainder);
				} break;
				case Direction::Vertical: {
					int total_element_heights = 0;
					for (Element& child : box->children) {
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
			for (Element& child : box->children) {
				/* Compute child position */
				Vector2 child_position = cursor;
				compute_element_positions(child_position, &child);

				/* Move cursor */
				switch (box->direction) {
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

	void layout_element(const ResourceManager& resources, Vector2 window_size, Element* element) {
		const Vector2 top_left = { 0, 0 };
		const Vector2 element_size = {
			.x = fit_size_to_parent(element->style.width, window_size.x),
			.y = fit_size_to_parent(element->style.height, window_size.y),
		};
		compute_element_sizes(resources, element_size, element);
		compute_element_positions(top_left, element);
	}

	void update_element(const Input& input, Element* element) {
		element->state.is_hovered = Raylib_CheckCollisionPointRec(input.mouse_pos, element->layout.border_box);
		element->state.is_active = element->state.is_hovered && input.left_mouse_is_down;
		if (ui::Box* box = std::get_if<ui::Box>(&element->content)) {
			for (Element& child : box->children) {
				update_element(input, &child);
			}
		}
	}

	void draw_element(const ResourceManager& resources, const Element& element) {
		const Style& style = element.style;

		/* Draw padding box */
		Color background_color = element.style.background_color.base;
		if (element.state.is_active) {
			background_color = element.style.background_color.active;
		} else if (element.state.is_hovered) {
			background_color = element.style.background_color.hovered;
		}
		Raylib_DrawRectangleRec(element.layout.padding_box, background_color);

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

		/* Debug draw box outlines */
		{
			if (element.style.debug.show_margin_outline) {
				Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, GREEN);
			}

			if (element.style.debug.show_content_outline) {
				Raylib_DrawRectangleLinesEx(element.layout.content_box, 1, GREEN);
			}
		}

		/* Draw content */
		if (const ui::Text* text = std::get_if<ui::Text>(&element.content)) {
			const Font& font = resources.get_font(style.font_id);
			const Rectangle content_box = element.layout.content_box;
			Raylib_BeginScissorMode(content_box.x, content_box.y, content_box.width, content_box.height);
			{
				int line_num = 0;
				for (const std::string& line : text->lines) {
					const float font_spacing = 0.0f;
					const int line_length = Raylib_MeasureTextEx(font, line.c_str(), style.font_size, font_spacing).x;
					const int left_padding = alignment_padding(style.alignment, content_box.width - line_length);
					Vector2 line_pos = {
						.x = element.layout.content_box.x + left_padding,
						.y = element.layout.content_box.y + line_num * style.font_size,
					};
					Raylib_DrawTextEx(font, line.c_str(), line_pos, style.font_size, font_spacing, style.font_color);
					line_num++;
				}
			}
			Raylib_EndScissorMode();
		} else if (const ui::Box* box = std::get_if<ui::Box>(&element.content)) {
			for (const Element& child : box->children) {
				draw_element(resources, child);
			}
		}
	}
}
