#include "game/scene/scene.h"

#include "game/game.h"
#include "game/scene/gameplay_scene.h"
#include "game/scene/main_menu_scene.h"
#include "game/scene/scene_id.h"

#include <variant>

using SceneInstance = std::variant<MainMenuScene, GameplayScene>;

// Scene class implementing variant based polymorphism
//
// To support DLL based hot reloading, we need to not store any DLL specific
// memory addresses in program state between hot reloads. V-table based
// polymorphism therefore doesn't work. Variants are stable between reloads.
//
// Scene class implemented in the cpp file to avoid re-compiling the entire
// game when a new scene has been added.
class Scene {
public:
	Scene(SceneID id) {
		m_id = id;
		switch (id) {
			case SceneID::MainMenu:
				m_instance = MainMenuScene();
				break;

			case SceneID::Gameplay:
				m_instance = GameplayScene();
				break;
		}
	}

	void initialize(Game* game) {
		std::visit([game](auto& scene) { scene.initialize(game); }, m_instance);
	}

	void deinitialize(Game* game) {
		std::visit([game](auto& scene) { scene.deinitialize(game); }, m_instance);
	}

	void update(Game* game) {
		std::visit([game](auto& scene) { scene.update(game); }, m_instance);
	}

	void render(const Game& game) const {
		std::visit([&game](auto& scene) { scene.render(game); }, m_instance);
	}

private:
	SceneID m_id;
	SceneInstance m_instance;
};

SceneManager::SceneManager() = default;

SceneManager::~SceneManager() = default;

void SceneManager::queue_push_scene(SceneID scene_id) {
	m_actions.push_back(PushScene { scene_id });
}

void SceneManager::queue_pop_scene() {
	m_actions.push_back(PopScene {});
}

void SceneManager::run_queued_actions(Game* game) {
	for (SceneAction action : m_actions) {
		if (PushScene* push_scene = std::get_if<PushScene>(&action)) {
			_push_scene(game, push_scene->scene_id);
		}
		if (PopScene* pop_scene = std::get_if<PopScene>(&action)) {
			_pop_scene(game);
		}
	}
	m_actions.clear();
}

void SceneManager::update_current_scene(Game* game) {
	if (!m_scenes.empty()) {
		m_scenes.back()->update(game);
	}
}

void SceneManager::render_current_scene(const Game& game) const {
	if (!m_scenes.empty()) {
		m_scenes.back()->render(game);
	}
}

void SceneManager::_push_scene(Game* game, SceneID scene_id) {
	m_scenes.push_back(std::make_unique<Scene>(scene_id));
	m_scenes.back()->initialize(game);
}

void SceneManager::_pop_scene(Game* game) {
	m_scenes.back()->deinitialize(game);
	if (m_scenes.size() == 1) {
		game->should_quit = true;
	} else {
		m_scenes.pop_back();
	}
}
