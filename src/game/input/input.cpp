#include "game/input/input.h"

ButtonState read_button_state(int button) {
	ButtonState state = ButtonState::Up;
	if (Raylib_IsMouseButtonPressed(button)) {
		state = ButtonState::Pressed;
	} else if (Raylib_IsMouseButtonDown(button)) {
		state = ButtonState::Down;
	} else if (Raylib_IsMouseButtonReleased(button)) {
		state = ButtonState::Released;
	} else if (Raylib_IsMouseButtonUp(button)) {
		state = ButtonState::Up;
	}
	return state;
}
