#pragma once

#include <raylib.h>

#include <unordered_map>

class Window;

enum class ButtonState {
	Up,
	Released,
	Down,
	Pressed,
};

struct Input {
	Vector2 mouse_position;
	std::unordered_map<MouseButton, ButtonState> mouse_buttons;

	ButtonState left_mouse_button() const {
		return mouse_buttons.at(MOUSE_BUTTON_LEFT);
	}

	bool left_mouse_button_up() const {
		return mouse_buttons.at(MOUSE_BUTTON_LEFT) == ButtonState::Up;
	}

	bool left_mouse_button_released() const {
		return mouse_buttons.at(MOUSE_BUTTON_LEFT) == ButtonState::Released;
	}

	bool left_mouse_button_down() const {
		return mouse_buttons.at(MOUSE_BUTTON_LEFT) == ButtonState::Down;
	}

	bool left_mouse_button_pressed() const {
		return mouse_buttons.at(MOUSE_BUTTON_LEFT) == ButtonState::Pressed;
	}
};

Input read_input(const Window& window);
