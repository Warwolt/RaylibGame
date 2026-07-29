#include "platform/input.h"

#include "core/debug/profiling.h"
#include "core/util.h"
#include "platform/window.h"

#include <raymath.h>

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

static ButtonState read_keyboard_key(int key) {
	ButtonState state = ButtonState::Up;
	if (Raylib_IsKeyPressed(key)) {
		state = ButtonState::Pressed;
	} else if (Raylib_IsKeyDown(key)) {
		state = ButtonState::Down;
	} else if (Raylib_IsKeyReleased(key)) {
		state = ButtonState::Released;
	} else if (Raylib_IsKeyUp(key)) {
		state = ButtonState::Up;
	}
	return state;
}

static ButtonState read_gamepad_button(int button) {
	const int gamepad_id = 0;
	ButtonState state = ButtonState::Up;
	if (Raylib_IsGamepadButtonPressed(gamepad_id, button)) {
		state = ButtonState::Pressed;
	} else if (Raylib_IsGamepadButtonDown(gamepad_id, button)) {
		state = ButtonState::Down;
	} else if (Raylib_IsGamepadButtonReleased(gamepad_id, button)) {
		state = ButtonState::Released;
	} else if (Raylib_IsGamepadButtonUp(gamepad_id, button)) {
		state = ButtonState::Up;
	}
	return state;
}

static float read_gamepad_stick(int axis) {
	const float deadzone = 0.1f;
	const float stick_value = Raylib_GetGamepadAxisMovement(0, axis);
	if (-deadzone < stick_value && stick_value < deadzone) {
		return 0.0f; // if within deadzone, pretend like stick is at rest
	}
	return stick_value;
}

std::unordered_map<InputAction, std::vector<KeyboardKey>> default_keyboard_bindings() {
	return {
		// clang-format off
		{ ACTION_UI_UP, { KEY_UP }  },
		{ ACTION_UI_LEFT, { KEY_LEFT } },
		{ ACTION_UI_DOWN, { KEY_DOWN } },
		{ ACTION_UI_RIGHT, { KEY_RIGHT } },
		{ ACTION_UI_SELECT, { KEY_ENTER, KEY_Z } },
		{ ACTION_UI_BACK, { KEY_ESCAPE, KEY_X } },
		// clang-format on
	};
}

std::unordered_map<InputAction, std::vector<GamepadButton>> default_gamepad_button_bindings() {
	return {
		// clang-format off
		{ ACTION_UI_UP, { GAMEPAD_BUTTON_LEFT_FACE_UP }  },
		{ ACTION_UI_LEFT, { GAMEPAD_BUTTON_LEFT_FACE_LEFT } },
		{ ACTION_UI_DOWN, { GAMEPAD_BUTTON_LEFT_FACE_DOWN } },
		{ ACTION_UI_RIGHT, { GAMEPAD_BUTTON_LEFT_FACE_RIGHT } },
		{ ACTION_UI_SELECT, { GAMEPAD_BUTTON_RIGHT_FACE_DOWN } },
		{ ACTION_UI_BACK, { GAMEPAD_BUTTON_RIGHT_FACE_RIGHT } },
		// clang-format on
	};
}

ButtonState Input::left_mouse_button() const {
	auto it = mouse_buttons.find(MOUSE_BUTTON_LEFT);
	return it == mouse_buttons.end() ? ButtonState::Up : it->second;
}

bool Input::left_mouse_button_up() const {
	return left_mouse_button() == ButtonState::Up;
}

bool Input::left_mouse_button_released() const {
	return left_mouse_button() == ButtonState::Released;
}

bool Input::left_mouse_button_down() const {
	return left_mouse_button() == ButtonState::Down;
}

bool Input::left_mouse_button_pressed() const {
	return left_mouse_button() == ButtonState::Pressed;
}

ButtonState Input::keyboard_key(KeyboardKey key) const {
	auto it = keyboard_keys.find(key);
	return it == keyboard_keys.end() ? ButtonState::Up : it->second;
}

bool Input::key_up(KeyboardKey key) const {
	return keyboard_key(key) == ButtonState::Up;
}

bool Input::key_released(KeyboardKey key) const {
	return keyboard_key(key) == ButtonState::Released;
}

bool Input::key_down(KeyboardKey key) const {
	return keyboard_key(key) == ButtonState::Down;
}

bool Input::key_pressed(KeyboardKey key) const {
	return keyboard_key(key) == ButtonState::Pressed;
}

ButtonState Input::gamepad_button(GamepadButton button) const {
	auto it = gamepad_buttons.find(button);
	return it == gamepad_buttons.end() ? ButtonState::Up : it->second;
}

bool Input::button_up(GamepadButton button) const {
	return gamepad_button(button) == ButtonState::Up;
}

bool Input::button_released(GamepadButton button) const {
	return gamepad_button(button) == ButtonState::Released;
}

bool Input::button_down(GamepadButton button) const {
	return gamepad_button(button) == ButtonState::Down;
}

bool Input::button_pressed(GamepadButton button) const {
	return gamepad_button(button) == ButtonState::Pressed;
}

ButtonState Input::input_action(InputAction action) const {
	auto it = input_actions.find(action);
	return it == input_actions.end() ? ButtonState::Up : it->second;
}

bool Input::action_up(InputAction action) const {
	return input_action(action) == ButtonState::Up;
}

bool Input::action_released(InputAction action) const {
	return input_action(action) == ButtonState::Released;
}

bool Input::action_down(InputAction action) const {
	return input_action(action) == ButtonState::Down;
}

bool Input::action_pressed(InputAction action) const {
	return input_action(action) == ButtonState::Pressed;
}

bool Input::last_input_was_mouse() const {
	return this->last_input_type == InputType::Mouse;
}

bool Input::last_input_was_keyboard() const {
	return this->last_input_type == InputType::Keyboard;
}

bool Input::last_input_was_gamepad() const {
	return this->last_input_type == InputType::Gamepad;
}

void read_input(Input* input, const Window& window) {
	PROFILING_SCOPE();

	/* Mouse */
	{
		const Vector2 global_mouse_position = Raylib_GetMousePosition();
		const Rectangle letterbox = window.letterbox();
		const int scale = window.letterbox_scale();
		input->mouse_position = {
			.x = (global_mouse_position.x - letterbox.x) / scale,
			.y = (global_mouse_position.y - letterbox.y) / scale,
		};

		bool any_mouse_button_pressed = false;
		for (int i = 0; i < 6; i++) {
			const ButtonState state = read_mouse_button(i);
			input->mouse_buttons[(MouseButton)i] = state;
			any_mouse_button_pressed |= (state == ButtonState::Pressed || state == ButtonState::Down);
		}

		const bool mouse_has_moved = Raylib_GetMouseDelta() != Vector2 { 0, 0 };
		if (any_mouse_button_pressed || mouse_has_moved) {
			input->last_input_type = InputType::Mouse;
		}
	}

	/* Keyboard */
	bool any_keyboard_key_pressed = false;
	{
		for (int i = 0; i < 336; i++) {
			const ButtonState state = read_keyboard_key(i);
			input->keyboard_keys[(KeyboardKey)i] = state;
			any_keyboard_key_pressed |= (state == ButtonState::Pressed || state == ButtonState::Down);
		}
		if (any_keyboard_key_pressed) {
			input->last_input_type = InputType::Keyboard;
		}
	}

	/* Gamepad */
	bool any_gamepad_button_pressed = false;
	{
		input->gamepad_left_stick_x = read_gamepad_stick(GAMEPAD_AXIS_LEFT_X);
		input->gamepad_left_stick_y = read_gamepad_stick(GAMEPAD_AXIS_LEFT_Y);
		input->gamepad_right_stick_x = read_gamepad_stick(GAMEPAD_AXIS_RIGHT_X);
		input->gamepad_right_stick_y = read_gamepad_stick(GAMEPAD_AXIS_RIGHT_Y);

		for (int i = 0; i < (int)GAMEPAD_BUTTON_RIGHT_THUMB; i++) {
			const ButtonState state = read_gamepad_button(i);
			input->gamepad_buttons[(GamepadButton)i] = state;
			any_gamepad_button_pressed |= (state == ButtonState::Pressed || state == ButtonState::Down);
		}
		if (any_gamepad_button_pressed) {
			input->last_input_type = InputType::Gamepad;
		}
	}

	/* Actions */
	{
		/* Keyboard */
		if (!any_gamepad_button_pressed) {
			for (auto& [action, keys] : input->keyboard_bindings) {
				if (util::any_of(keys, [&](KeyboardKey key) { return input->key_down(key); })) {
					input->input_actions[action] = ButtonState::Down;
				} else if (util::any_of(keys, [&](KeyboardKey key) { return input->key_pressed(key); })) {
					input->input_actions[action] = ButtonState::Pressed;
				} else if (util::any_of(keys, [&](KeyboardKey key) { return input->key_up(key); })) {
					input->input_actions[action] = ButtonState::Up;
				} else if (util::any_of(keys, [&](KeyboardKey key) { return input->key_released(key); })) {
					input->input_actions[action] = ButtonState::Released;
				}
			}
		}

		/* Gamepad buttons */
		if (!any_keyboard_key_pressed) {
			for (auto& [action, buttons] : input->gamepad_button_bindings) {
				if (util::any_of(buttons, [&](GamepadButton button) { return input->button_down(button); })) {
					input->input_actions[action] = ButtonState::Down;
				} else if (util::any_of(buttons, [&](GamepadButton button) { return input->button_pressed(button); })) {
					input->input_actions[action] = ButtonState::Pressed;
				} else if (util::any_of(buttons, [&](GamepadButton button) { return input->button_up(button); })) {
					input->input_actions[action] = ButtonState::Up;
				} else if (util::any_of(buttons, [&](GamepadButton button) { return input->button_released(button); })) {
					input->input_actions[action] = ButtonState::Released;
				}
			}

			/* Gamepad sticks */
			// flicking thumb stick navigates in UI
			if (input->gamepad_left_stick_x.previous() == 0 && input->gamepad_left_stick_y == 0) {
				if (input->gamepad_left_stick_x < 0) {
					input->input_actions[ACTION_UI_LEFT] = ButtonState::Pressed;
				}
				if (input->gamepad_left_stick_x > 0) {
					input->input_actions[ACTION_UI_RIGHT] = ButtonState::Pressed;
				}
			}
			if (input->gamepad_left_stick_y.previous() == 0 && input->gamepad_left_stick_x == 0) {
				if (input->gamepad_left_stick_y < 0) {
					input->input_actions[ACTION_UI_UP] = ButtonState::Pressed;
				}
				if (input->gamepad_left_stick_y > 0) {
					input->input_actions[ACTION_UI_DOWN] = ButtonState::Pressed;
				}
			}
		}

		/* Time */
		input->time_now = Time::now();
	}
}
