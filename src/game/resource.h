#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

struct FontID {
	static FontID default_font() {
		return FontID { 0 };
	};

	int value = 0;
};

class ResourceManager {
public:
	ResourceManager() = default;

	void load_default_font(const std::string& path);
	const Font& default_font() const;

private:
	Font m_default_font;
};
