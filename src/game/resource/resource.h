#pragma once

#include "game/resource/resource_id.h"
#include "game/resource/sprite_sheet.h"

#include <raylib.h>

#include <optional>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
	ResourceManager() = default;

	void load_default_font(const std::string& path);
	Font get_font(FontID font_id) const;

	std::optional<ImageID> load_image(const std::string& path);
	const Texture2D& get_image(ImageID image_id) const;

	std::optional<SpriteSheetID> load_aseprite_sprite_sheet(const std::string& image_path, const std::string& json_path);
	const SpriteSheet get_sprite_sheet(SpriteSheetID sprite_sheet_id) const;

	std::optional<SoundID> load_sound(const std::string& path);
	const Sound& get_sound(SoundID sound_id) const;


private:
	Font m_default_font = {};

	int m_next_image_id = 1;
	std::unordered_map<int, Texture2D> m_images;
	inline static const Texture2D m_empty_image = {};

	int m_next_sprite_sheet_id = 1;
	std::unordered_map<int, SpriteSheet> m_sprite_sheets;
	inline static const SpriteSheet m_empty_sprite_sheet = {};

	int m_next_sound_id = 1;
	std::unordered_map<int, Sound> m_sounds;
	inline static const Sound m_empty_sound = {};
};
