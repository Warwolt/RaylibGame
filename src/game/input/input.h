#pragma once

#include "game/input/button.h"

#include <raylib.h>

struct Input {
	Vector2 mouse_pos;
	ButtonState left_mouse_button;
};

ButtonState read_button_state(int button); // FIXME: make this private?
