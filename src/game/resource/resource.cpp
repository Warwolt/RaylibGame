#include "game/resource/resource.h"

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

std::optional<SpriteSheetID> ResourceManager::load_aseprite_sprite_sheet(const std::string& image_path, const std::string& json_path) {
	/* Load sprite sheet image */
	std::optional<ImageID> image_id = load_image(image_path);
	if (!image_id.has_value()) {
		LOG_ERROR("Couldn't open file %s, can't load sprite sheet", image_path.c_str());
		return {};
	}

	/* Load sprite sheet json data */
	std::ifstream sprite_sheet_json_file = std::ifstream(json_path);
	if (!sprite_sheet_json_file.is_open()) {
		LOG_ERROR("Couldn't open file %s, can't load sprite sheet", json_path.c_str());
		return {};
	}

	/* Parse json data */
	nlohmann::json sprite_sheet_json = nlohmann::json::parse(sprite_sheet_json_file);
	if (sprite_sheet_json.is_discarded()) {
		LOG_ERROR("Couldn't parse json file %s, can't load sprite sheet", json_path.c_str());
	}

	/* Read frames */
	if (!sprite_sheet_json["frames"].is_array()) {
		LOG_ERROR("%s is missing a \"frames\" array", json_path.c_str());
		return {};
	}
	std::vector<Rectangle> frames;
	for (const nlohmann::json& frame_json : sprite_sheet_json["frames"]) {
		frames.push_back(
			Rectangle {
				.x = frame_json["frame"]["x"].get<float>(),
				.y = frame_json["frame"]["y"].get<float>(),
				.width = frame_json["frame"]["w"].get<float>(),
				.height = frame_json["frame"]["h"].get<float>(),
			}
		);
	}

	/* Read animations */
	if (!sprite_sheet_json["meta"].is_object()) {
		LOG_ERROR("%s is missing a \"meta\" object", json_path.c_str());
		return {};
	}
	if (!sprite_sheet_json["meta"]["frameTags"].is_array()) {
		LOG_ERROR("%s is missing a \"frameTags\" array in the \"meta\" object", json_path.c_str());
		return {};
	}
	std::unordered_map<std::string, Animation<int>> animations;
	for (const nlohmann::json& frame_tag : sprite_sheet_json["meta"]["frameTags"]) {
		Animation<int> animation;
		const int from = frame_tag["from"].get<int>();
		const int to = frame_tag["to"].get<int>();
		for (int i = from; i <= to; i++) {
			int duration_ms = sprite_sheet_json["frames"][i]["duration"].get<int>();
			animation.push_back(
				AnimationFrame<int> {
					.value = i,
					.duration = std::chrono::milliseconds(duration_ms),
				}
			);
		}
		animations.insert({ frame_tag["name"].get<std::string>(), animation });
	}

	/* Add sprite sheet */
	SpriteSheet sprite_sheet = {
		.image_id = image_id.value(),
		.frames = std::move(frames),
		.animations = std::move(animations),
	};
	SpriteSheetID id = SpriteSheetID { m_next_sprite_sheet_id++ };
	m_sprite_sheets.insert({ id.value, std::move(sprite_sheet) });

	return id;
}

const SpriteSheet ResourceManager::get_sprite_sheet(SpriteSheetID sprite_sheet_id) const {
	if (auto it = m_sprite_sheets.find(sprite_sheet_id.value); it != m_sprite_sheets.end()) {
		return it->second;
	}

	LOG_ERROR("Missing image for SpriteSheetID(%d)", sprite_sheet_id.value);
	return m_empty_sprite_sheet;
}

std::optional<SoundID> ResourceManager::load_sound(const std::string& path) {
	Sound sound = Raylib_LoadSound(path.c_str());
	if (!Raylib_IsSoundValid(sound)) {
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
