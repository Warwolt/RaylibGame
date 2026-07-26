#include "game/ui/style.h"

#include "game/ui/context.h"
#include "game/ui/element.h"

namespace ui {

#define SELECT_STYLE_OVERRIDE(style, field, state)                                   \
	select_style_override(style.field, style.hover.field, style.active.field, state)

	template <typename T>
	static T select_style_override(const T& normal, const std::optional<T>& hover, const std::optional<T>& active, StyleState state) {
		if (state == StyleState::Active) {
			return active.value_or(normal);
		}
		if (state == StyleState::Hover) {
			return hover.value_or(normal);
		}
		return normal;
	}

	StyleState get_style_state(const Context& context, const Element& element) {
		if (context.is_active(element)) {
			return StyleState::Active;
		}
		if (context.is_hovered(element)) {
			return StyleState::Hover;
		}
		return StyleState::Inactive;
	}

	Style resolve_style_for_state(const Style& style, StyleState state) {
		return Style {
			.position = style.position,
			.width = style.width,
			.height = style.height,
			.fit_content = style.fit_content,
			.margin = style.margin,
			.border = {
				.edges = style.border.edges,
				.color = SELECT_STYLE_OVERRIDE(style, border.color, state),
				.image = SELECT_STYLE_OVERRIDE(style, border.image, state),
				.image_slices = SELECT_STYLE_OVERRIDE(style, border.image_slices, state),
				.image_fill_center = SELECT_STYLE_OVERRIDE(style, border.image_fill_center, state),
			},
			.padding = style.padding,
			.alignment = style.alignment,
			.cross_alignment = style.cross_alignment,
			.background = {
				.color = SELECT_STYLE_OVERRIDE(style, background.color, state),
				.image = SELECT_STYLE_OVERRIDE(style, background.image, state),
				.fill = SELECT_STYLE_OVERRIDE(style, background.fill, state),
			},
			.font = {
				.id = SELECT_STYLE_OVERRIDE(style, font.id, state),
				.size = SELECT_STYLE_OVERRIDE(style, font.size, state),
				.color = SELECT_STYLE_OVERRIDE(style, font.color, state),
			},
			.hover = style.hover,
			.active = style.active,
			.debug = style.debug,
		};
	}

} // namespace ui
