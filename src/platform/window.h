#pragma once

#include "platform/lean_mean_windows.h"

#include <raylib.h>

// Represents a window at a given resolution.
// Even if the application window is re-sized, the viewport remains the same size.
// Render logic is based on the viewport, not the application window.
class Window {
public:
	Window() = default;
	static Window initialize(int width, int height);
	void deinitialize();

	int width() const;
	int height() const;
	Vector2 size() const;
	RenderTexture viewport() const;
	Rectangle letterbox() const;

	void update();
	void toggle_fullscreen();

private:
	WINDOWPLACEMENT m_placement; // used for fullscreen toggling
	RenderTexture m_viewport; // used to draw the game
	Rectangle m_letterbox; // used to draw the viewport to the application window
	int m_scale; // letterbox:viewport scale
};
