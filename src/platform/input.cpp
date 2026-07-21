#include "platform/input.h"

#include "platform/window.h"

static ButtonState read_mouse_button(int button) {
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
	Input input;

	/* Mouse */
	const Vector2 global_mouse_position = Raylib_GetMousePosition();
	const Rectangle letterbox = window.letterbox();
	const int scale = window.letterbox_scale();
	input.mouse_position = {
		.x = (global_mouse_position.x - letterbox.x) / scale,
		.y = (global_mouse_position.y - letterbox.y) / scale,
	};
	for (int i = 0; i < 6; i++) {
		input.mouse_buttons[(MouseButton)i] = read_mouse_button(i);
	}

	/* Keyboard */


	return input;
}
