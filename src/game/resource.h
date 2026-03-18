#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
	ResourceManager() = default;

	void load_default_font(const std::string& path);
	const Font& default_font() const;

private:
	Font m_default_font;
};
