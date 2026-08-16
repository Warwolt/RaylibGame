#pragma once

struct FontID {
	int value = 0;

	static FontID default_font() {
		return FontID { 0 };
	};

	bool operator==(const FontID& rhs) const = default;
};

struct ImageID {
	int value = 0;

	bool operator==(const ImageID& rhs) const = default;
};

struct SoundID {
	int value = 0;

	bool operator==(const SoundID& rhs) const = default;
};
