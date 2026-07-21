#pragma once

#include <raylib.h>

class Window;

enum class ButtonState {
	Up,
	Released,
	Down,
	Pressed,
};

struct Input {
	Vector2 mouse_position;
	ButtonState left_mouse_button;
};

Input read_input(const Window& window);
