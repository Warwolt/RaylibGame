#include "game/debug_overlay.h"

#include "game/game.h"
#include "hot_reload.h"

#include <raylib.h>

using namespace std::chrono_literals;

void DebugOverlay::initialize() {
	m_overlay_text_animation.set_animation(
		{
			{ "Rebuilding", 500ms },
			{ "Rebuilding.", 500ms },
			{ "Rebuilding..", 500ms },
			{ "Rebuilding...", 500ms },
		}
	);
}

void DebugOverlay::update(Game* game) {
	if (game->hot_reload_state.has_changed_to(HotReloadState::Rebuilding)) {
		m_overlay_text_animation.start(Time::now());
	}

	if (game->hot_reload_state.has_changed_from(HotReloadState::Rebuilding)) {
		m_overlay_text_animation.stop();
	}

	switch (game->hot_reload_state) {
		case HotReloadState::Rebuilding:
			m_overlay_text_animation.update(Time::now());
			m_overlay_text = m_overlay_text_animation.value();
			m_overlay_text_color = YELLOW;
			break;

		case HotReloadState::Failed:
			m_overlay_text = "Rebuild Failed!";
			m_overlay_text_color = RED;
			break;

		case HotReloadState::ReadyToReload:
		case HotReloadState::Idle:
			m_overlay_text = "";
			break;
	}
}

void DebugOverlay::render(const Game& game) const {
	if (!m_overlay_text.empty()) {
		Font font = game.resources.get_font(FontID::default_font());
		Raylib_DrawRectangle(0, 0, game.window.width(), game.window.height(), Color { 0, 0, 0, 127 });
		Raylib_DrawTextEx(font, m_overlay_text.c_str(), { 4, 0 }, 16, 0, m_overlay_text_color);
	}
}
