#include "game/resource.h"

void ResourceManager::load_default_font(const std::string& path) {
	m_default_font = Raylib_LoadFont(path.c_str());
}

// TODO: use font_id to look up a font
Font ResourceManager::get_font(FontID /*font_id*/) const {
	if (m_default_font.texture.id != 0) {
		return m_default_font;
	} else {
		return Raylib_GetFontDefault();
	}
}
