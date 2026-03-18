#include "game/scene/scene.h"

#include "game/game.h"

Scene::Scene(SceneID id) {
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

void Scene::update(Game* game) {
	switch (m_id) {
		case SceneID::MainMenu:
			std::get_if<MainMenuScene>(&m_state)->update(game);
			break;

		case SceneID::Gameplay:
			std::get_if<GameplayScene>(&m_state)->update(game);
			break;
	}
}

void Scene::render(const Game& game) const {
	switch (m_id) {
		case SceneID::MainMenu:
			std::get_if<MainMenuScene>(&m_state)->render(game);
			break;

		case SceneID::Gameplay:
			std::get_if<GameplayScene>(&m_state)->render(game);
			break;
	}
}

SceneManager::SceneManager(SceneID start_scene_id) {
	m_scenes.push_back(Scene(start_scene_id));
}

void SceneManager::push_scene(Game* /*game*/, SceneID scene_id) {
	m_scenes.push_back(scene_id);
	// FIXME: initialize the scene
}

void SceneManager::pop_scene(Game* game) {
	if (m_scenes.size() == 1) {
		game->should_quit = true;
	} else {
		m_scenes.pop_back();
	}
}

Scene& SceneManager::current_scene() {
	// FIXME: assert m_scenes non-empty
	return m_scenes.back();
}

const Scene& SceneManager::current_scene() const {
	// FIXME: assert m_scenes non-empty
	return m_scenes.back();
}
