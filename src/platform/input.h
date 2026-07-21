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
	std::unordered_map<KeyboardKey, ButtonState> keyboard_keys;

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

	ButtonState keyboard_key(KeyboardKey key) const {
		auto it = keyboard_keys.find(key);
		return it == keyboard_keys.end() ? ButtonState::Up : it->second;
	}
	bool key_up(KeyboardKey key) const {
		return keyboard_key(key) == ButtonState::Up;
	}
	bool key_released(KeyboardKey key) const {
		return keyboard_key(key) == ButtonState::Released;
	}
	bool key_down(KeyboardKey key) const {
		return keyboard_key(key) == ButtonState::Down;
	}
	bool key_pressed(KeyboardKey key) const {
		return keyboard_key(key) == ButtonState::Pressed;
	}
};

Input read_input(const Window& window);
