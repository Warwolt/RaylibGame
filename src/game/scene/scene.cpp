#include "game/scene/scene.h"

#include "game/game.h"
#include "game/scene/gameplay.h"
#include "game/scene/main_menu.h"

#include <variant>

using SceneState = std::variant<MainMenuScene, GameplayScene>;

class Scene {
public:
	Scene(SceneID id) {
		m_id = id;
		switch (id) {
			case SceneID::MainMenu:
				m_state = MainMenuScene();
				break;

			case SceneID::Gameplay:
				m_state = GameplayScene();
				break;
		}
	}

	void initialize(Game* game) {
		std::visit([game](auto& scene) { scene.initialize(game); }, m_state);
	}

	void deinitialize(Game* game) {
		std::visit([game](auto& scene) { scene.deinitialize(game); }, m_state);
	}

	void update(Game* game) {
		std::visit([game](auto& scene) { scene.update(game); }, m_state);
	}

	void render(const Game& game) const {
		std::visit([&game](auto& scene) { scene.render(game); }, m_state);
	}

private:
	SceneID m_id;
	SceneState m_state;
};

SceneManager::SceneManager(SceneID start_scene_id) {
	m_scenes.push_back(Scene(start_scene_id));
}

SceneManager::~SceneManager() = default;

void SceneManager::push_scene(Game* game, SceneID scene_id) {
	m_scenes.push_back(Scene(scene_id));
	m_scenes.back().initialize(game);
}

void SceneManager::pop_scene(Game* game) {
	m_scenes.back().deinitialize(game);
	if (m_scenes.size() == 1) {
		game->should_quit = true;
	} else {
		m_scenes.pop_back();
	}
}

void SceneManager::update_current_scene(Game* game) {
	m_scenes.back().update(game);
}

void SceneManager::render_current_scene(const Game& game) const {
	m_scenes.back().render(game);
}
