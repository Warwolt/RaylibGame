#pragma once

#include "core/util/time.h"
#include "core/util/tracked.h"

#include <raylib.h>

#include <unordered_map>

class Window;

enum class ButtonState {
	Up,
	Released,
	Down,
	Pressed,
};

// C style enum to match Raylib style
typedef enum {
	ACTION_UI_UP,
	ACTION_UI_LEFT,
	ACTION_UI_DOWN,
	ACTION_UI_RIGHT,
	ACTION_UI_SELECT,
	ACTION_UI_BACK,
} InputAction;

enum class InputType {
	Mouse,
	Keyboard,
	Gamepad,
};

std::unordered_map<InputAction, std::vector<KeyboardKey>> default_keyboard_bindings();
std::unordered_map<InputAction, std::vector<GamepadButton>> default_gamepad_button_bindings();

struct Input {
	/* Mouse */
	Vector2 mouse_position;
	std::unordered_map<MouseButton, ButtonState> mouse_buttons;

	ButtonState left_mouse_button() const;
	bool left_mouse_button_up() const;
	bool left_mouse_button_released() const;
	bool left_mouse_button_down() const;
	bool left_mouse_button_pressed() const;

	/* Keyboard */
	std::unordered_map<KeyboardKey, ButtonState> keyboard_keys;
	std::unordered_map<InputAction, std::vector<KeyboardKey>> keyboard_bindings = default_keyboard_bindings();
	ButtonState keyboard_key(KeyboardKey key) const;
	bool key_up(KeyboardKey key) const;
	bool key_released(KeyboardKey key) const;
	bool key_down(KeyboardKey key) const;
	bool key_pressed(KeyboardKey key) const;

	/* Gamepad */
	Tracked<float> gamepad_left_stick_x;
	Tracked<float> gamepad_left_stick_y;
	Tracked<float> gamepad_right_stick_x;
	Tracked<float> gamepad_right_stick_y;
	std::unordered_map<GamepadButton, ButtonState> gamepad_buttons;
	std::unordered_map<InputAction, std::vector<GamepadButton>> gamepad_button_bindings = default_gamepad_button_bindings();
	ButtonState gamepad_button(GamepadButton button) const;
	bool button_up(GamepadButton button) const;
	bool button_released(GamepadButton button) const;
	bool button_down(GamepadButton button) const;
	bool button_pressed(GamepadButton button) const;

	/* Logical */
	std::unordered_map<InputAction, ButtonState> input_actions;
	InputType last_input_type = InputType::Keyboard;

	ButtonState input_action(InputAction action) const;
	bool action_up(InputAction action) const;
	bool action_released(InputAction action) const;
	bool action_down(InputAction action) const;
	bool action_pressed(InputAction action) const;

	bool last_input_was_mouse() const;
	bool last_input_was_keyboard() const;
	bool last_input_was_gamepad() const;

	/* Time */
	Time time_now = {};
};

void read_input(Input* input, const Window& window);
