#pragma once

#include "game/resource_id.h"

#include <optional>
#include <raylib.h>
#include <string>
#include <unordered_map>

// FIXME: make this easier to use in tests
//
// It would be nice to make this _just_ a map
// So, we'd have `add_image` and `get_image`
// To load an image from diks, we'd have a helper function
// std::optional<ImageID> load_image(ResourceManager* resources, const std::string& path);
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
