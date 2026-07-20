#pragma once

#include "game/resource.h"
#include "game/ui/element.h"
#include "game/ui/tree.h"

#include <raylib.h>

#include <optional>
#include <string>
#include <string_view>

namespace ui {

	class UserInterface {
	public:
		void draw(const ResourceManager& resources) const;
		const Element& root_element() const;

		void frame_begin();
		void frame_end(const ResourceManager& resources, Vector2 window_size);

		void box_begin(Direction direction = Direction::Vertical, std::optional<Style> style = {}, std::string id = "");
		void box_end();

		void text(std::string_view text, std::optional<Style> style = {}, std::string id = "");
		void image(ImageID image, std::optional<Style> style = {}, std::string id = "");

	private:
		bool m_is_within_frame = false;
		Context m_context;
		Tree m_tree;
	};

} // namespace ui
