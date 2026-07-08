#include "game/ui.h"

#include "core/debug/assert.h"
#include "core/debug/profiling.h"
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

	static bool size_is_100_percent(Size size) {
		if (RelativeSize* relative_size = std::get_if<RelativeSize>(&size)) {
			return relative_size->percentage == 100;
		}
		return false;
	}

	static float fit_size_to_parent(const Size& size, float parent_size) {
		float pixels = 0.0f;
		if (const AbsoluteSize* absolute_size = std::get_if<AbsoluteSize>(&size)) {
			pixels = std::min<float>(absolute_size->pixels, parent_size);
		}
		if (const RelativeSize* relative_size = std::get_if<RelativeSize>(&size)) {
			pixels = (relative_size->percentage / 100.0f) * parent_size;
		}
		return pixels;
	}

	// based on Raylib MeasureTextEx in rtext.c
	static int measure_word_width(std::string_view word, const Font& font, int font_size, int font_spacing) {
		if (font.texture.id == 0) {
			return 0;
		}

		int word_width = 0;
		for (char letter : word) {
			const int index = GetGlyphIndex(font, letter);
			if (font.glyphs[index].advanceX > 0) {
				word_width += font.glyphs[index].advanceX;
			} else {
				word_width += (font.recs[index].width + font.glyphs[index].offsetX);
			}
		}

		const float scale_factor = font_size / (float)font.baseSize;
		return word_width * scale_factor + font_spacing * word.length() - 1;
	}

	static Vector2 compute_desired_element_size(const ResourceManager& resources, Vector2 parent_size, Element* element) {
		Vector2 desired_size = { 0, 0 };
		const Style& style = element->style;

		if (Text* text = element->text()) {
			const Font& font = resources.get_font(style.font_id);
			const float font_spacing = 0.0f;
			const float element_width = fit_size_to_parent(style.width, parent_size.x);
			const float element_height = fit_size_to_parent(style.height, parent_size.y);
			const float max_text_width = element_width - style.horizontal_spacing();
			const float max_text_height = element_height - style.vertical_spacing();
			const int space_width = Raylib_MeasureTextEx(font, " ", style.font_size, font_spacing).x;
			/* Fit text to element size */
			Vector2 cursor = { 0, 0 };
			text->lines.clear();
			for (const std::string_view word : util::get_string_view_per_word(text->text)) {
				const int word_width = measure_word_width(word, font, style.font_size, font_spacing);
				const int needed_length = cursor.x > 0 ? space_width + word_width : word_width;
				// check if word fits on remainder of current line
				if (cursor.x + needed_length <= max_text_width) {
					// extend current line view to include word
					if (cursor.x > 0) {
						cursor.x += space_width;
					}
					if (text->lines.empty()) {
						text->lines.push_back(word);
					} else {
						const char* start = text->lines.back().data();
						text->lines.back() = std::string_view(start, (word.data() + word.size()) - start);
					}
					cursor.x += word_width;

				} else {
					// switch to new line
					cursor.x = 0;
					cursor.y = cursor.y + style.font_size;
					if (cursor.y + style.font_size > max_text_height) {
						break;
					}
					text->lines.push_back(word);
					cursor.x = word_width;
				}
			}
			const float paragraph_height = cursor.y + style.font_size;
			desired_size = {
				.x = element_width,
				.y = paragraph_height + style.vertical_spacing(),
			};
		} else if (element->is_image()) {
			desired_size = {
				.x = fit_size_to_parent(style.width, parent_size.x),
				.y = fit_size_to_parent(style.height, parent_size.y),
			};
		} else if (element->is_box()) {
			desired_size = {
				.x = fit_size_to_parent(style.width, parent_size.x),
				.y = fit_size_to_parent(style.height, parent_size.y),
			};
		} else {
			ABORT("Unhandled ui::Content case!");
		}

		return desired_size;
	}

	static void compute_constrained_element_sizes(const ResourceManager& resources, Vector2 max_size, Element* element) {
		const Style& style = element->style;
		Layout* layout = &element->layout;

		/* Compute size of content box */
		if (element->is_text()) {
			layout->content_box.width = max_size.x - style.horizontal_spacing();
			layout->content_box.height = max_size.y - style.vertical_spacing();
		} else if (element->is_image()) {
			layout->content_box.width = max_size.x - style.horizontal_spacing();
			layout->content_box.height = max_size.y - style.vertical_spacing();
		} else if (Box* box = element->box()) {
			/* Size parent content  */
			Rectangle& content_box = layout->content_box;
			content_box.width = max_size.x - style.horizontal_spacing();
			content_box.height = max_size.y - style.vertical_spacing();

			/* Recursively size all box children */
			{
				// 1. compute desired size of each child
				struct IndexedVector2 {
					size_t index;
					Vector2 value;
				};
				std::vector<IndexedVector2> desired_sizes;
				for (size_t i = 0; i < box->children.size(); i++) {
					Element& child = box->children[i];
					Vector2 parent_size = { content_box.width, content_box.height };
					Vector2 desired_size = compute_desired_element_size(resources, parent_size, &child);
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
						compute_constrained_element_sizes(resources, child_size, &child);
					} else {
						const Vector2 child_size = {
							.x = content_box.width,
							.y = std::min<float>(desired_size.value.y, remaining_height / remaining_children),
						};
						compute_constrained_element_sizes(resources, child_size, &child);
						remaining_height -= child.layout.margin_box.height;
					}
				}
			}
		} else {
			ABORT("Unhandled ui::Content case!");
		}

		/* Size padding, border, and margin boxes */
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
		if (Box* box = element->box()) {
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
		PROFILING_SCOPE();
		const Vector2 top_left = { 0, 0 };
		const Vector2 desired_size = compute_desired_element_size(resources, window_size, element);
		compute_constrained_element_sizes(resources, desired_size, element);
		compute_element_positions(top_left, element);
	}

	static bool key_is_down(ButtonState state) {
		return state == ButtonState::Down || state == ButtonState::Pressed;
	}

	bool update_element(const Input& input, Element* element) {
		/* Hovered */
		element->state.is_hovered = Raylib_CheckCollisionPointRec(input.mouse_pos, element->layout.border_box);

		/* Active */
		if (element->state.is_active) {
			// element stays active as long as button is held down
			element->state.is_active = key_is_down(input.left_mouse_button);
		} else {
			// element becomes active if pressed while hovered
			element->state.is_active = element->state.is_hovered && input.left_mouse_button == ButtonState::Pressed;
		}

		/* Update children */
		bool any_child_clicked = false;
		if (ui::Box* box = element->box()) {
			for (Element& child : box->children) {
				any_child_clicked |= update_element(input, &child);
			}
		}

		/* Clicked */
		if (any_child_clicked) {
			element->state.is_clicked = true; // propagate clicks
		} else {
			element->state.is_clicked = element->state.is_hovered && input.left_mouse_button == ButtonState::Released;
		}
		const bool click_handled = any_child_clicked || element->state.is_clicked;

		return click_handled;
	}

	void draw_element(const ResourceManager& resources, const Element& element) {
		/* Draw padding box */
		Color background_color = element.style.background_color;
		if (element.state.is_active) {
			background_color = element.style.active.background_color.value_or(background_color);
		} else if (element.state.is_hovered) {
			background_color = element.style.hovered.background_color.value_or(background_color);
		}
		Raylib_DrawRectangleRec(element.layout.padding_box, background_color);

		/* Draw border */
		{
			Color border_color = element.style.border_color;
			if (element.state.is_active) {
				border_color = element.style.active.border_color.value_or(border_color);
			} else if (element.state.is_hovered) {
				border_color = element.style.hovered.border_color.value_or(border_color);
			}

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
			Raylib_DrawRectangleRec(border_top, border_color);
			Raylib_DrawRectangleRec(border_bottom, border_color);
			Raylib_DrawRectangleRec(border_left, border_color);
			Raylib_DrawRectangleRec(border_right, border_color);
		}

		/* Debug draw box outlines */
		{
			if (element.style.debug.show_margin_outline) {
				Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, ORANGE);
			}

			if (element.style.debug.show_content_outline) {
				Raylib_DrawRectangleLinesEx(element.layout.content_box, 1, GREEN);
			}
		}

		/* Draw background image */
		if (element.style.background_image.value != 0) {
			Texture2D texture = resources.get_image(element.style.background_image);
			Rectangle source = {
				.x = 0,
				.y = 0,
				.width = (float)texture.width,
				.height = (float)texture.height,
			};
			DrawTexturePro(texture, source, element.layout.content_box, Vector2 { 0, 0 }, 0.0f, WHITE);
		}

		/* Draw content */
		if (const ui::Text* text = element.text()) {
			const Font& font = resources.get_font(element.style.font_id);
			const Rectangle content_box = element.layout.content_box;
			Raylib_BeginScissorMode(content_box.x, content_box.y, content_box.width, content_box.height);
			{
				int line_num = 0;
				for (const std::string_view line : text->lines) {
					const float font_spacing = 0.0f;
					const int line_length = measure_word_width(line, font, element.style.font_size, font_spacing);
					const int left_padding = alignment_padding(element.style.alignment, content_box.width - line_length);
					Vector2 line_pos = {
						.x = element.layout.content_box.x + left_padding,
						.y = element.layout.content_box.y + line_num * element.style.font_size,
					};
					Color font_color = element.style.font_color;
					if (element.state.is_active) {
						font_color = element.style.active.font_color.value_or(font_color);
					} else if (element.state.is_hovered) {
						font_color = element.style.hovered.font_color.value_or(font_color);
					}
					const std::string line_str(line);
					Raylib_DrawTextEx(font, line_str.c_str(), line_pos, element.style.font_size, font_spacing, font_color);
					line_num++;
				}
			}
			Raylib_EndScissorMode();
		} else if (const Image* image = element.image()) {
			Texture2D texture = resources.get_image(image->image);
			float source_width = 0;
			if (const AbsoluteSize* absolute_width = std::get_if<AbsoluteSize>(&element.style.width)) {
				source_width = std::min((float)absolute_width->pixels, element.layout.content_box.width);
			}
			if (std::holds_alternative<RelativeSize>(element.style.width)) {
				source_width = texture.width;
			}
			float source_height = 0;
			if (const AbsoluteSize* absolute_height = std::get_if<AbsoluteSize>(&element.style.height)) {
				source_height = std::min((float)absolute_height->pixels, element.layout.content_box.height);
			}
			if (std::holds_alternative<RelativeSize>(element.style.height)) {
				source_height = texture.height;
			}

			Rectangle source = {
				.x = 0,
				.y = 0,
				.width = source_width,
				.height = source_height,
			};
			DrawTexturePro(texture, source, element.layout.content_box, Vector2 { 0, 0 }, 0.0f, WHITE);
		} else if (const Box* box = element.box()) {
			for (const Element& child : box->children) {
				draw_element(resources, child);
			}
		} else {
			ABORT("Unhandled ui::Content case!");
		}
	}

	void UserInterface::draw(const ResourceManager& resources) const {
		draw_element(resources, m_root_element);
	}

	const Element& UserInterface::root_element() const {
		return m_root_element;
	}

	void UserInterface::frame_begin() {
		ASSERT(!m_within_frame, "Missing call to UserInterface::frame_end?");
		m_within_frame = true;
		m_root_element = {};
		m_parent_stack = { &m_root_element };
	}

	void UserInterface::frame_end(const ResourceManager& resources, Vector2 window_size) {
		PROFILING_SCOPE();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(m_parent_stack.size() == 1, "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_within_frame = false;
		layout_element(resources, window_size, &m_root_element);
	}

	void UserInterface::box_begin(Direction direction, std::optional<Style> style) {
		Element* parent = _current_parent();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		parent->box()->children.push_back(
			Element {
				.style = style.value_or(Style {}),
				.content = Box { .direction = direction },
			}
		);
		m_parent_stack.push_back(&parent->box()->children.back());
	}

	void UserInterface::box_end() {
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		ASSERT(m_parent_stack.size() > 1, "UserInterface::box_begin and box_end calls don't match. Missing call to UserInterface::box_end?");
		m_parent_stack.pop_back();
	}

	void UserInterface::text(std::string_view text, std::optional<Style> style) {
		Element* parent = _current_parent();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		parent->box()->children.push_back(
			Element {
				.style = style.value_or(Style {}),
				.content = Text { .text = std::string(text) },
			}
		);
	}

	void UserInterface::image(ImageID image, std::optional<Style> style) {
		Element* parent = _current_parent();
		ASSERT(m_within_frame, "Missing call to UserInterface::frame_begin?");
		parent->box()->children.push_back(
			Element {
				.style = style.value_or(Style {}),
				.content = Image { .image = image },
			}
		);
	}

	Element* UserInterface::_current_parent() {
		ASSERT(!m_parent_stack.empty(), "Forgot to add root element to parent stack?");
		return m_parent_stack.back();
	}
}
