#include "platform/window.h"

Window Window::initialize(int width, int height) {
	Window window;
	window.m_viewport = Raylib_LoadRenderTexture(width, height);
	return window;
}

void Window::deinitialize() {
	Raylib_UnloadRenderTexture(m_viewport);
}

int Window::width() const {
	return m_viewport.texture.width;
}

int Window::height() const {
	return m_viewport.texture.height;
}

Vector2 Window::size() const {
	return Vector2 {
		.x = (float)m_viewport.texture.width,
		.y = (float)m_viewport.texture.height,
	};
}

RenderTexture Window::viewport() const {
	return m_viewport;
}

Rectangle Window::letterbox() const {
	return m_letterbox;
}

void Window::update() {
	int screen_width = Raylib_GetScreenWidth();
	int screen_height = Raylib_GetScreenHeight();
	int viewport_width = m_viewport.texture.width;
	int viewport_height = m_viewport.texture.height;

	/* Update the letterbox for the viewport */
	// The "letterbox" is the rectangle in the application window where the
	// viewport is being rendered. The viewport is upscaled as much as possible
	// to fit the window size with maintained aspect ratio, with black matte
	// added to the sides if needed.
	m_scale = min(screen_width / viewport_width, screen_height / viewport_height);
	float scaled_width = (float)(m_scale * viewport_width);
	float scaled_height = (float)(m_scale * viewport_height);
	m_letterbox = Rectangle {
		.x = (screen_width - scaled_width) / 2.0f,
		.y = (screen_height - scaled_height) / 2.0f,
		.width = scaled_width,
		.height = scaled_height,
	};
}

// Based on Raymond Chen's "How do I switch a window between normal and fullscreen?"
// https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
void Window::toggle_fullscreen() {
	HWND handle = (HWND)Raylib_GetWindowHandle();

	DWORD style = GetWindowLong(handle, GWL_STYLE);
	if (style & WS_OVERLAPPEDWINDOW) {
		GetWindowPlacement(handle, &m_placement);
		SetWindowLong(handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO monitor_info = { .cbSize = sizeof(monitor_info) };
		GetMonitorInfo(MonitorFromWindow(handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
		LONG monitor_x = monitor_info.rcMonitor.left;
		LONG monitor_y = monitor_info.rcMonitor.top;
		LONG monitor_width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
		LONG monitor_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
		SetWindowPos(handle, HWND_TOP, monitor_x, monitor_y, monitor_width, monitor_height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	} else {
		SetWindowLong(handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME);
		SetWindowPlacement(handle, &m_placement);
		SetWindowPos(handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}
