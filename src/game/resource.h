#pragma once

#include <optional>
#include <raylib.h>
#include <string>
#include <unordered_map>

struct FontID {
	int value = 0;

	static FontID default_font() {
		return FontID { 0 };
	};
};

struct ImageID {
	int value = 0;
};

class ResourceManager {
public:
	ResourceManager() = default;

	void load_default_font(const std::string& path);
	Font get_font(FontID font_id) const;

	std::optional<ImageID> load_image(const std::string& path);
	Texture2D get_image(ImageID image_id) const;

private:
	Font m_default_font = {};

	int m_next_image_id = 1;
	std::unordered_map<int, Texture2D> m_images;
};
