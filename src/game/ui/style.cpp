#include "game/ui/style.h"

#include "game/ui/context.h"
#include "game/ui/element.h"

namespace ui {

	StyleState get_style_state(const Context& context, const Element& element) {
		if (context.is_active(element)) {
			return StyleState::Active;
		}
		if (context.is_hovered(element)) {
			return StyleState::Hovered;
		}
		return StyleState::Inactive;
	}

} // namespace ui
