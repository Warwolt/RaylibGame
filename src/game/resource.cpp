#include "game/resource.h"

#include "core/debug/logging.h"

void ResourceManager::load_default_font(const std::string& path) {
	m_default_font = Raylib_LoadFont(path.c_str());
}

const Font& ResourceManager::default_font() const {
	return m_default_font;
}
