#include "game/ui/element.h"

#include "core/debug/assert.h"
#include "core/debug/profiling.h"
#include "core/util.h"
#include "game/resource.h"
#include "platform/input.h"

#include <algorithm>
#include <array>

namespace ui {

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
		return word_width * scale_factor + font_spacing * (word.length() - 1);
	}

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

	// returns slices of box with given spacing
	static std::array<Rectangle, 9> edges_to_9_slices(const Edges& edges, const Rectangle& box) {
		const float top_height = edges.top;
		const float middle_height = box.height - edges.top - edges.bottom;
		const float bottom_height = edges.bottom;
		const float center_width = box.width - edges.left - edges.right;
		const float top_y = box.y;
		const float middle_y = box.y + edges.top;
		const float bottom_y = box.height - edges.bottom;
		return {
			Rectangle { box.x, top_y, edges.left, top_height }, // top left
			Rectangle { box.x + edges.left, top_y, center_width, top_height }, // top center
			Rectangle { box.x + box.width - edges.right, top_y, edges.right, top_height }, // top right

			Rectangle { box.x, middle_y, edges.left, middle_height }, // middle left
			Rectangle { box.x + edges.left, middle_y, center_width, middle_height }, // middle center
			Rectangle { box.x + box.width - edges.right, middle_y, edges.right, middle_height }, // middle right

			Rectangle { box.x, bottom_y, edges.left, bottom_height }, // bottom left
			Rectangle { box.x + edges.left, bottom_y, center_width, bottom_height }, // bottom center
			Rectangle { box.x + box.width - edges.right, bottom_y, edges.right, bottom_height }, // bottom right
		};
	}

	// The size of an element only considering its content.
	static Measure2 get_intrinsic_content_size(const ResourceManager& resources, const Element& element, const Style& resolved_style) {
		if (const Text* text = element.text()) {
			// Intrinsic size of text element is just one long line
			const Font font = resources.get_font(resolved_style.font.id);
			const int font_size = resolved_style.font.size;
			const float width = measure_word_width(text->text, font, font_size, 0);
			return {
				Pixels(width),
				Pixels(font_size),
			};
		} else if (const Image* image = element.image()) {
			// Intrinsic size of image is the size of the texture itself
			Texture2D texture = resources.get_image(image->id);
			return {
				Pixels(texture.width),
				Pixels(texture.height),
			};
		} else if (element.is_box()) {
			return {
				Percentage(100),
				Percentage(100),
			};
		} else {
			ABORT("Missing ui::Content case!");
			return {
				Percentage(100),
				Percentage(100),
			};
		}
	}

	// computes space that the box children will use, given the layout direction
	static Vector2 compute_child_content_size(const Box& box) {
		Vector2 content_size = {};
		for (const Element& child : box.children) {
			if (child.style.position.is_absolute_position()) {
				continue; // remove absolutely positioned element from flow
			}
			switch (box.direction) {
				case Direction::Horizontal: {
					content_size.x += child.layout.margin_box.width;
					content_size.y = std::max(content_size.y, child.layout.margin_box.height);
				} break;
				case Direction::Vertical: {
					content_size.x = std::max(content_size.x, child.layout.margin_box.width);
					content_size.y += child.layout.margin_box.height;
				} break;
			}
		}
		return content_size;
	}

	// compute desired size of margin box
	static Vector2 compute_desired_element_size(const ResourceManager& resources, const Context& context, Vector2 parent_size, Element* element) {
		Vector2 desired_size = { 0, 0 };
		const StyleState state = get_style_state(context, *element);
		const Style resolved_style = resolve_style_for_state(element->style, state);
		const Measure2 intrinsic_size = get_intrinsic_content_size(resources, *element, resolved_style);
		const float max_content_width = parent_size.x - resolved_style.horizontal_spacing();
		const float max_content_height = parent_size.y - resolved_style.vertical_spacing();

		if (Text* text = element->text()) {
			const Font font = resources.get_font(resolved_style.font.id);
			const float font_spacing = 0.0f;
			const int space_width = Raylib_MeasureTextEx(font, " ", resolved_style.font.size, font_spacing).x;

			// The actual paragraph width, might be smaller than content area
			float paragraph_width = 0;
			const Measure& content_width = resolved_style.width.value_or(intrinsic_size.x);
			if (const Pixels* pixel_width = content_width.pixels()) {
				paragraph_width = std::min<float>(pixel_width->value, max_content_width);
			}
			if (const Percentage* percentage_width = content_width.percentage()) {
				paragraph_width = percentage_width->fractional() * max_content_width;
			}

			/* Fit text to element size */
			Vector2 cursor = { 0, 0 };
			text->lines.clear();
			for (const std::string_view word : util::get_string_view_per_word(text->text)) {
				const int word_width = measure_word_width(word, font, resolved_style.font.size, font_spacing);
				const int needed_length = cursor.x > 0 ? space_width + word_width : word_width;
				// check if word fits on remainder of current line
				if (cursor.x + needed_length <= paragraph_width) {
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
					cursor.y = cursor.y + resolved_style.font.size;
					if (cursor.y + resolved_style.font.size > max_content_height) {
						break;
					}
					text->lines.push_back(word);
					cursor.x = word_width;
				}
			}

			const float paragraph_height = cursor.y + resolved_style.font.size;
			const bool has_pixel_height = resolved_style.height.has_value() && resolved_style.height->is_pixels();
			const float content_height = has_pixel_height ? resolved_style.height->pixels()->value : paragraph_height;

			desired_size = {
				.x = paragraph_width + resolved_style.horizontal_spacing(),
				.y = content_height + resolved_style.vertical_spacing(),
			};
		} else if (element->is_image() || element->is_box()) {
			/* Constrain style width */
			const Measure& content_width = resolved_style.width.value_or(intrinsic_size.x);
			if (const Pixels* pixel_width = content_width.pixels()) {
				if (resolved_style.position.is_absolute_position()) {
					// Absolutely positioned elements sizes aren't constrained
					desired_size.x = pixel_width->value + resolved_style.horizontal_spacing();
				} else {
					const float margin_pixel_width = pixel_width->value + resolved_style.horizontal_spacing();
					desired_size.x = std::min<float>(margin_pixel_width, parent_size.x);
				}
			}
			if (const Percentage* percentage_width = content_width.percentage()) {
				desired_size.x = percentage_width->fractional() * parent_size.x + resolved_style.horizontal_spacing();
			}
			/* Constrain style height */
			const Measure& content_height = resolved_style.height.value_or(intrinsic_size.x);
			if (const Pixels* pixel_height = content_height.pixels()) {
				if (resolved_style.position.is_absolute_position()) {
					// Absolutely positioned elements sizes aren't constrained
					desired_size.y = pixel_height->value + resolved_style.vertical_spacing();
				} else {
					const float margin_pixel_height = pixel_height->value + resolved_style.vertical_spacing();
					desired_size.y = std::min<float>(margin_pixel_height, parent_size.y);
				}
			}
			if (const Percentage* percentage_height = content_height.percentage()) {
				desired_size.y = percentage_height->fractional() * parent_size.y + resolved_style.vertical_spacing();
			}
		} else {
			ABORT("Unhandled ui::Content case!");
		}

		return desired_size;
	}

	// `max_size` is the constraint on the margin box size
	static void compute_constrained_element_sizes(const ResourceManager& resources, const Context& context, Vector2 max_size, Element* element) {
		const StyleState state = get_style_state(context, *element);
		const Style resolved_style = resolve_style_for_state(element->style, state);
		Layout* layout = &element->layout;

		/* Compute size of content box */
		if (element->is_text() || element->is_image()) {
			layout->content_box.width = max_size.x - resolved_style.horizontal_spacing();
			layout->content_box.height = max_size.y - resolved_style.vertical_spacing();
		} else if (Box* box = element->box()) {
			/* Compute max size of parent content box */
			const Vector2 max_parent_size = {
				max_size.x - resolved_style.horizontal_spacing(),
				max_size.y - resolved_style.vertical_spacing(),
			};

			//
			// FIXME: this code needs to be cleaned up
			//
			// Box children with absolute position are just given their desired size directly.
			// Other children should participate in size fitting relative to parent content area.
			// Right now the code looks a little messy to me.
			//
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
					const Vector2 desired_size = compute_desired_element_size(resources, context, max_parent_size, &child);
					if (child.style.position.is_absolute_position()) {
						// Absolutely positioned elements get their desired size directly
						compute_constrained_element_sizes(resources, context, desired_size, &child);
					} else {
						desired_sizes.push_back({ i, desired_size });
					}
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
				float remaining_width = max_parent_size.x;
				float remaining_height = max_parent_size.y;
				for (size_t i = 0; i < desired_sizes.size(); i++) {
					const size_t remaining_children = desired_sizes.size() - i;
					const IndexedVector2& desired_size = desired_sizes[i];
					Element& child = box->children[desired_size.index];
					if (box->direction == Direction::Horizontal) {
						const Vector2 child_size = {
							.x = std::min<float>(desired_size.value.x, remaining_width / remaining_children),
							.y = std::min<float>(desired_size.value.y, max_parent_size.y),
						};
						remaining_width -= child_size.x;
						compute_constrained_element_sizes(resources, context, child_size, &child);
					} else {
						const Vector2 child_size = {
							.x = std::min<float>(desired_size.value.x, max_parent_size.x),
							.y = std::min<float>(desired_size.value.y, remaining_height / remaining_children),
						};
						compute_constrained_element_sizes(resources, context, child_size, &child);
						remaining_height -= child.layout.margin_box.height;
					}
				}
			}

			/* Size parent content */
			Rectangle& content_box = layout->content_box;
			if (resolved_style.fit_content) {
				const Vector2 child_content_size = compute_child_content_size(*box);
				content_box.width = child_content_size.x;
				content_box.height = child_content_size.y;
			} else {
				content_box.width = max_parent_size.x;
				content_box.height = max_parent_size.y;
			}
		} else {
			ABORT("Unhandled ui::Content case!");
		}

		/* Size padding, border, and margin boxes */
		// clang-format off
		layout->padding_box.width = layout->content_box.width + resolved_style.padding.left + resolved_style.padding.right;
		layout->padding_box.height = layout->content_box.height + resolved_style.padding.top + resolved_style.padding.bottom;
		layout->border_box.width = layout->padding_box.width + resolved_style.border.edges.left + resolved_style.border.edges.right;
		layout->border_box.height = layout->padding_box.height + resolved_style.border.edges.top + resolved_style.border.edges.bottom;
		layout->margin_box.width = layout->border_box.width + resolved_style.margin.left + resolved_style.margin.right;
		layout->margin_box.height = layout->border_box.height + resolved_style.margin.top + resolved_style.margin.bottom;
		// clang-format on
	}

	// `containing_box` is either root or the closest parent element with a non-static position
	static void compute_element_positions(const Context& context, Vector2 position, Rectangle containing_box, Element* element) {
		const StyleState state = get_style_state(context, *element);
		const Style resolved_style = resolve_style_for_state(element->style, state);
		Layout* layout = &element->layout;

		/* Compute position offsets */
		Vector2 offset = { 0, 0 };
		if (const RelativePosition* relative_position = resolved_style.position.relative_position()) {
			if (const Pixels* x_pixels = relative_position->x.pixels()) {
				offset.x = x_pixels->value;
			}
			if (const Percentage* x_percentage = relative_position->x.percentage()) {
				offset.x = x_percentage->fractional() * element->layout.border_box.width;
			}

			if (const Pixels* y_pixels = relative_position->y.pixels()) {
				offset.y = y_pixels->value;
			}
			if (const Percentage* y_percentage = relative_position->y.percentage()) {
				offset.y = y_percentage->fractional() * element->layout.border_box.height;
			}
		}

		/* Position all boxes relative to each other */
		if (const AbsolutePosition* absolute_position = resolved_style.position.absolute_position()) {
			if (const Pixels* x_pixels = absolute_position->x.pixels()) {
				layout->margin_box.x = containing_box.x + x_pixels->value;
			}
			if (const Percentage* x_percentage = absolute_position->x.percentage()) {
				layout->margin_box.x = containing_box.x + x_percentage->fractional() * containing_box.width;
			}
			if (const Pixels* y_pixels = absolute_position->y.pixels()) {
				layout->margin_box.y = containing_box.y + y_pixels->value;
			}
			if (const Percentage* y_percentage = absolute_position->y.percentage()) {
				layout->margin_box.y = containing_box.y + y_percentage->fractional() * containing_box.height;
			}
		} else {
			layout->margin_box.x = position.x + offset.x;
			layout->margin_box.y = position.y + offset.y;
		}
		layout->border_box.x = layout->margin_box.x + resolved_style.margin.left;
		layout->border_box.y = layout->margin_box.y + resolved_style.margin.top;
		layout->padding_box.x = layout->border_box.x + resolved_style.border.edges.left;
		layout->padding_box.y = layout->border_box.y + resolved_style.border.edges.top;
		layout->content_box.x = layout->padding_box.x + resolved_style.padding.left;
		layout->content_box.y = layout->padding_box.y + resolved_style.padding.top;

		/* Recurse into children */
		if (Box* box = element->box()) {
			/* Compute padding for alignment on main axis */
			float left_padding = 0;
			float top_padding = 0;
			{
				const Vector2 child_content_size = compute_child_content_size(*box);
				const int horizontal_remainder = element->layout.content_box.width - child_content_size.x;
				const int vertical_remainder = element->layout.content_box.height - child_content_size.y;
				switch (box->direction) {
					case Direction::Horizontal: {
						left_padding = alignment_padding(resolved_style.alignment, horizontal_remainder);
					} break;

					case Direction::Vertical: {
						top_padding = alignment_padding(resolved_style.alignment, vertical_remainder);
					} break;
				}
			}

			/* Iterate over children */
			Vector2 cursor = {
				.x = layout->content_box.x + left_padding,
				.y = layout->content_box.y + top_padding,
			};
			for (Element& child : box->children) {
				/* Compute per-child cross-axis padding */
				float child_left_padding = 0;
				float child_top_padding = 0;
				if (!child.style.position.is_absolute_position()) {
					switch (box->direction) {
						case Direction::Horizontal: {
							const float remainder = element->layout.content_box.height - child.layout.margin_box.height;
							child_top_padding = alignment_padding(resolved_style.cross_alignment, remainder);
						} break;
						case Direction::Vertical: {
							const float remainder = element->layout.content_box.width - child.layout.margin_box.width;
							child_left_padding = alignment_padding(resolved_style.cross_alignment, remainder);
						} break;
					}
				}

				/* Compute child position */
				const Vector2 child_position = {
					.x = cursor.x + child_left_padding,
					.y = cursor.y + child_top_padding,
				};
				const Rectangle child_containing_box =
					resolved_style.position.is_static_position() ? containing_box : element->layout.margin_box;
				compute_element_positions(context, child_position, child_containing_box, &child);

				/* Move cursor */
				if (child.style.position.is_absolute_position()) {
					continue; // don't move cursor for absolutely positioned elements
				}
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

	void layout_element(const ResourceManager& resources, const Context& context, Vector2 window_size, Element* element) {
		PROFILING_SCOPE();
		const Rectangle containing_box = { 0, 0, window_size.x, window_size.y };
		const Vector2 top_left = { 0, 0 };
		const Vector2 desired_size = compute_desired_element_size(resources, context, window_size, element);
		const Vector2 max_size = {
			.x = std::min(desired_size.x, containing_box.width),
			.y = std::min(desired_size.y, containing_box.height),
		};
		compute_constrained_element_sizes(resources, context, max_size, element);
		compute_element_positions(context, top_left, containing_box, element);
	}

	bool update_element(const Input& input, Context* context, Element* element) {
		State* state = context->state(*element);

		/* Update hovered & active if element has associated state */
		if (state) {
			/* Hovered */
			state->is_hovered = Raylib_CheckCollisionPointRec(input.mouse_position, element->layout.border_box);

			/* Active */
			if (state->is_active) {
				// element stays active as long as button is held down
				state->is_active = input.left_mouse_button_pressed() || input.left_mouse_button_down();
			} else {
				// element becomes active if pressed while hovered
				state->is_active = state->is_hovered && input.left_mouse_button_pressed();
			}
		}

		/* Update children */
		bool any_child_clicked = false;
		if (ui::Box* box = element->box()) {
			/* Recurse through child child */
			std::optional<size_t> focused_child_index;
			for (size_t i = 0; i < box->children.size(); i++) {
				Element& child = box->children[i];
				/* Update child */
				any_child_clicked |= update_element(input, context, &child);
				if (context->is_focused(child)) {
					focused_child_index = i;
				}
			}

			/* Update focus */
			// Note: We do this outside the above loop so we don't update focus
			// more than once per frame when we recurse through the children.
			if (focused_child_index.has_value()) {
				const bool box_is_horizontal = box->direction == Direction::Horizontal;
				const bool should_focus_previous = box_is_horizontal ? input.action_pressed(ACTION_UI_LEFT) : input.action_pressed(ACTION_UI_UP);
				const bool should_focus_next = box_is_horizontal ? input.action_pressed(ACTION_UI_RIGHT) : input.action_pressed(ACTION_UI_DOWN);

				// search among  siblings, focus first one that has an id
				if (should_focus_previous) {
					for (int i = (int)focused_child_index.value() - 1; i >= 0; i--) {
						Element& sibling = box->children[i];
						if (!sibling.id.empty()) {
							context->focus_element(sibling);
							break;
						}
					}
				}
				if (should_focus_next) {
					for (size_t i = focused_child_index.value() + 1; i < box->children.size(); i++) {
						Element& sibling = box->children[i];
						if (!sibling.id.empty()) {
							context->focus_element(sibling);
							break;
						}
					}
				}
			}
		}

		/* Update clicked if element has associated state */
		bool click_handled = any_child_clicked;
		if (state) {
			if (any_child_clicked) {
				state->is_clicked = true; // propagate clicks
				click_handled = true;
			} else {
				const bool element_is_mouse_clicked = state->is_hovered && input.left_mouse_button_released();
				const bool element_is_keyboard_clicked = context->is_focused(*element) && input.action_pressed(ACTION_UI_SELECT);
				const bool is_clicked = element_is_mouse_clicked || element_is_keyboard_clicked;
				state->is_clicked = is_clicked;
				click_handled = is_clicked;
			}
		}

		return click_handled;
	}

	void draw_element(const ResourceManager& resources, const Context& context, const Element& element) {
		const StyleState state = get_style_state(context, element);
		const Style resolved_style = resolve_style_for_state(element.style, state);
		const auto& [intrinsic_width, intrinsic_height] = get_intrinsic_content_size(resources, element, resolved_style);

		/* Draw padding box */
		Raylib_DrawRectangleRec(element.layout.padding_box, resolved_style.background.color);

		/* Draw border */
		{
			const Rectangle border_top = {
				.x = element.layout.border_box.x,
				.y = element.layout.border_box.y,
				.width = element.layout.border_box.width,
				.height = resolved_style.border.edges.top,
			};
			const Rectangle border_bottom = {
				.x = element.layout.border_box.x,
				.y = element.layout.padding_box.y + element.layout.padding_box.height,
				.width = element.layout.border_box.width,
				.height = resolved_style.border.edges.bottom,
			};
			const Rectangle border_left = {
				.x = element.layout.border_box.x,
				.y = element.layout.padding_box.y,
				.width = resolved_style.border.edges.left,
				.height = element.layout.padding_box.height,
			};
			const Rectangle border_right = {
				.x = element.layout.border_box.x + element.layout.border_box.width - resolved_style.border.edges.right,
				.y = element.layout.padding_box.y,
				.width = resolved_style.border.edges.right,
				.height = element.layout.padding_box.height,
			};
			Raylib_DrawRectangleRec(border_top, resolved_style.border.color);
			Raylib_DrawRectangleRec(border_bottom, resolved_style.border.color);
			Raylib_DrawRectangleRec(border_left, resolved_style.border.color);
			Raylib_DrawRectangleRec(border_right, resolved_style.border.color);
		}

		/* Draw border image */
		if (resolved_style.border.image.value != 0) {
			const Texture2D texture = resources.get_image(resolved_style.border.image);
			const Edges& slice_spacing = resolved_style.border.image_slices;
			const Rectangle texture_rect = { 0, 0, texture.width, texture.height };
			const auto source_rects = edges_to_9_slices(slice_spacing, texture_rect);
			const auto destination_rects = edges_to_9_slices(resolved_style.border.edges, element.layout.border_box);
			for (size_t i = 0; i < source_rects.size(); i++) {
				if (i == 4 && !resolved_style.border.image_fill_center) {
					continue;
				}
				Raylib_DrawTexturePro(texture, source_rects[i], destination_rects[i], Vector2 { 0, 0 }, 0.0f, WHITE);
			}
		}

		/* Debug draw box outlines */
		{
			if (resolved_style.debug.show_margin_outline) {
				Raylib_DrawRectangleLinesEx(element.layout.margin_box, 1, ORANGE);
			}

			if (resolved_style.debug.show_content_outline) {
				Raylib_DrawRectangleLinesEx(element.layout.content_box, 1, GREEN);
			}
		}

		/* Draw background image */
		if (resolved_style.background.image.value != 0) {
			Texture2D texture = resources.get_image(resolved_style.background.image);
			switch (resolved_style.background.fill) {
				case Fill::Repeat: {
					Rectangle source = {
						.x = 0,
						.y = 0,
						.width = element.layout.content_box.width,
						.height = element.layout.content_box.height,
					};
					Raylib_DrawTexturePro(texture, source, element.layout.content_box, Vector2 { 0, 0 }, 0.0f, WHITE);
				} break;

				case Fill::Stretch: {
					Rectangle source = {
						.x = 0,
						.y = 0,
						.width = (float)texture.width,
						.height = (float)texture.height,
					};
					Raylib_DrawTexturePro(texture, source, element.layout.content_box, Vector2 { 0, 0 }, 0.0f, WHITE);
				} break;
			}
		}

		/* Draw content */
		if (const ui::Text* text = element.text()) {
			const Font font = resources.get_font(resolved_style.font.id);
			const Rectangle content_box = element.layout.content_box;
			Raylib_BeginScissorMode(content_box.x, content_box.y, content_box.width, content_box.height);
			{
				int line_num = 0;
				const int text_height = (int)text->lines.size() * resolved_style.font.size;
				const int top_padding = alignment_padding(resolved_style.cross_alignment, content_box.height - text_height);
				for (const std::string_view line : text->lines) {
					const float font_spacing = 0.0f;
					const int line_length = measure_word_width(line, font, resolved_style.font.size, font_spacing);
					const int left_padding = alignment_padding(resolved_style.alignment, content_box.width - line_length);
					Vector2 line_pos = {
						.x = element.layout.content_box.x + left_padding,
						.y = element.layout.content_box.y + line_num * resolved_style.font.size + top_padding,
					};
					Color font_color = resolved_style.font.color;
					if (context.is_active(element)) {
						font_color = resolved_style.active.font.color.value_or(font_color);
					} else if (context.is_hovered(element)) {
						font_color = resolved_style.hover.font.color.value_or(font_color);
					}
					const std::string line_str(line);
					Raylib_DrawTextEx(font, line_str.c_str(), line_pos, resolved_style.font.size, font_spacing, font_color);
					line_num++;
				}
			}
			Raylib_EndScissorMode();
		} else if (const Image* image = element.image()) {
			const Texture2D texture = resources.get_image(image->id);

			// Handle overflow of absolutely sized images
			//
			// Depending on the size of the content box and the desired size of
			// the image, we have to sample either the whole image texture or
			// just a portion of it (in case of an overflow).
			float source_width = (float)texture.width;
			float source_height = (float)texture.height;
			if (const Pixels* pixel_width = resolved_style.width.value_or(intrinsic_width).pixels()) {
				if (pixel_width->value > element.layout.content_box.width) {
					const float scale = texture.width / pixel_width->value;
					source_width = scale * element.layout.content_box.width;
				}
			}
			if (const Pixels* pixel_height = resolved_style.height.value_or(intrinsic_height).pixels()) {
				if (pixel_height->value > element.layout.content_box.height) {
					const float scale = texture.height / pixel_height->value;
					source_height = scale * element.layout.content_box.height;
				}
			}

			const Rectangle source = {
				.x = 0,
				.y = 0,
				.width = source_width,
				.height = source_height,
			};
			Raylib_DrawTexturePro(texture, source, element.layout.content_box, Vector2 { 0, 0 }, 0.0f, WHITE);
		} else if (const Box* box = element.box()) {
			/* Draw statically and relatively positioned children first */
			for (const Element& child : box->children) {
				if (!child.style.position.is_absolute_position()) {
					draw_element(resources, context, child);
				}
			}
			/* Draw absolutely positioned children last */
			for (const Element& child : box->children) {
				if (child.style.position.is_absolute_position()) {
					draw_element(resources, context, child);
				}
			}
		} else {
			ABORT("Unhandled ui::Content case!");
		}
	}

} // namespace ui
