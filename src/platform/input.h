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
		auto it = mouse_buttons.find(MOUSE_BUTTON_LEFT);
		return it == mouse_buttons.end() ? ButtonState::Up : it->second;
	}

	bool left_mouse_button_up() const {
		return left_mouse_button() == ButtonState::Up;
	}

	bool left_mouse_button_released() const {
		return left_mouse_button() == ButtonState::Released;
	}

	bool left_mouse_button_down() const {
		return left_mouse_button() == ButtonState::Down;
	}

	bool left_mouse_button_pressed() const {
		return left_mouse_button() == ButtonState::Pressed;
	}
};

Input read_input(const Window& window);
