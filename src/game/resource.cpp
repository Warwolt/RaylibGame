#include "game/resource.h"

#include "core/debug/logging.h"

void ResourceManager::load_default_font(const std::string& path) {
	m_default_font = Raylib_LoadFont(path.c_str());
}

const Font& ResourceManager::default_font() const {
	return m_default_font;
}

const Font& ResourceManager::get_font(FontID /*font_id*/) const {
	// TODO: use font_id to look up a font
	return m_default_font;
}
