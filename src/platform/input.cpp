#include "platform/input.h"

#include "platform/window.h"

static ButtonState read_button_state(int button) {
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

Input read_input(const Window& window) {
	const Vector2 global_mouse_position = Raylib_GetMousePosition();
	const Rectangle letterbox = window.letterbox();
	const int scale = window.letterbox_scale();
	const Vector2 letterbox_mouse_position = {
		.x = (global_mouse_position.x - letterbox.x) / scale,
		.y = (global_mouse_position.y - letterbox.y) / scale,
	};

	return {
		.mouse_position = letterbox_mouse_position,
		.left_mouse_button = read_button_state(MOUSE_BUTTON_LEFT),
	};
}
