#pragma once

#include "game/resource.h"
#include "game/ui/element.h"
#include "game/ui/tree.h"

#include <raylib.h>

#include <optional>
#include <string>
#include <string_view>

struct Game;

namespace ui {

	class UserInterface {
	public:
		void initialize(Game* game);

		void draw(const ResourceManager& resources) const;
		const Element& root_element() const;

		void frame_begin();
		void frame_end(const Input& input, const ResourceManager& resources, Vector2 window_size);

		/* Primitive elements */
		void box_begin(std::optional<Style> style = {}, std::string id = "");
		void box_end();
		void text(std::string_view text, std::optional<Style> style = {}, std::string id = "");
		void image(ImageID image, std::optional<Style> style = {}, std::string id = "");

		/* Menu elements */
		void menu_begin(std::optional<Style> style = {});
		bool menu_item(const Input& input, const ResourceManager& resources, std::string_view label);
		void menu_end();

		/* Element state */
		Tracked<bool> element_is_hovered() const;
		Tracked<bool> element_is_focused() const;
		Tracked<bool> element_is_active() const;
		Tracked<bool> element_is_clicked() const;

		void initially_focus_current_element();
		void initially_focus_next_element();
		void focus_current_element();

	private:
		void _push_element(Element element);

		struct Images {
			ImageID focus_indicator;
		} m_images;

		struct Sounds {
			SoundID menu_navigate;
		} m_sounds;

		bool m_is_within_frame = false;
		Context m_context;
		Tree m_tree;
		bool m_should_initially_focus_next_element = false;
	};

} // namespace ui
