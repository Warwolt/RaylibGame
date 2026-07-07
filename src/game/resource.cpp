#include "game/resource.h"

#include "core/debug/logging.h"

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

std::optional<ImageID> ResourceManager::load_image(const std::string& path) {
	Texture2D texture = Raylib_LoadTexture(path.c_str());
	if (texture.id == 0) {
		return {};
	}
	ImageID id = ImageID { m_next_image_id++ };
	m_images.insert({ id.value, texture });
	return id;
}

Texture2D ResourceManager::get_image(ImageID image_id) const {
	if (auto it = m_images.find(image_id.value); it != m_images.end()) {
		return it->second;
	}
	
	LOG_ERROR("Missing image for ID %d", image_id.value);
	return {};
}
