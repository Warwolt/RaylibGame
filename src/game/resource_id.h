#pragma once

struct FontID {
	int value = 0;

	static FontID default_font() {
		return FontID { 0 };
	};
};

struct ImageID {
	int value = 0;
};
