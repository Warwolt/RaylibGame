#pragma once

#include "game/resource_id.h"

#include <optional>
#include <raylib.h>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
	ResourceManager() = default;

	void load_default_font(const std::string& path);
	Font get_font(FontID font_id) const;

	std::optional<ImageID> load_image(const std::string& path);
	Texture2D get_image(ImageID image_id) const;

	std::optional<SoundID> load_sound(const std::string& path);
	Sound get_sound(SoundID sound_id) const;

private:
	Font m_default_font = {};

	int m_next_image_id = 1;
	std::unordered_map<int, Texture2D> m_images;

	int m_next_sound_id = 1;
	std::unordered_map<int, Sound> m_sounds;
};
