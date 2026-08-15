#include "game/resource.h"

#include "core/debug/logging.h"

#include <nlohmann/json.hpp>

#include <fstream>

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
		LOG_ERROR("Couldn't load image %s", path.c_str());
		return {};
	}
	ImageID id = ImageID { m_next_image_id++ };
	m_images.insert({ id.value, texture });
	return id;
}

const Texture2D& ResourceManager::get_image(ImageID image_id) const {
	if (auto it = m_images.find(image_id.value); it != m_images.end()) {
		return it->second;
	}

	LOG_ERROR("Missing image for ImageID(%d)", image_id.value);
	return m_empty_image;
}

std::optional<SoundID> ResourceManager::load_sound(const std::string& path) {
	Sound sound = Raylib_LoadSound(path.c_str());
	if (!Raylib_IsSoundValid(sound)) {
		LOG_ERROR("Couldn't load sound %s", path.c_str());
		return {};
	}
	SoundID id = SoundID { m_next_sound_id++ };
	m_sounds.insert({ id.value, sound });
	return id;
}

const Sound& ResourceManager::get_sound(SoundID sound_id) const {
	if (auto it = m_sounds.find(sound_id.value); it != m_sounds.end()) {
		return it->second;
	}

	LOG_ERROR("Missing image for SoundID(%d)", sound_id.value);
	return m_empty_sound;
}
