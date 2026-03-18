#include "game/scene/scene.h"

#include "game/game.h"

#include <functional>

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

void Scene::initialize(Game* game) {
	switch (m_id) {
		case SceneID::MainMenu:
			std::get_if<MainMenuScene>(&m_state)->initialize(game);
			break;

		case SceneID::Gameplay:
			std::get_if<GameplayScene>(&m_state)->initialize(game);
			break;
	}
}

void Scene::deinitialize(Game* game) {
	switch (m_id) {
		case SceneID::MainMenu:
			std::get_if<MainMenuScene>(&m_state)->deinitialize(game);
			break;

		case SceneID::Gameplay:
			std::get_if<GameplayScene>(&m_state)->deinitialize(game);
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

void SceneManager::push_scene(Game* game, SceneID scene_id) {
	m_scenes.push_back(scene_id);
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
