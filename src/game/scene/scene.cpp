#include "game/scene/scene.h"

#include "game/game.h"

#include <raylib.h>

Scene::Scene(SceneID id) {
	this->id = id;
	switch (id) {
		case SceneID::MainMenu:
			this->state = MainMenuScene();
			break;
		case SceneID::Gameplay:
			this->state = GameplayScene();
			break;
	}
}

void Scene::update(Game* game) {
	switch (this->id) {
		case SceneID::MainMenu:
			std::get_if<MainMenuScene>(&this->state)->update(game);
			break;

		case SceneID::Gameplay:
			std::get_if<GameplayScene>(&this->state)->update(game);
			break;
	}
}

void Scene::render(const Game& game) const {
	switch (this->id) {
		case SceneID::MainMenu:
			std::get_if<MainMenuScene>(&this->state)->render(game);
			break;

		case SceneID::Gameplay:
			std::get_if<GameplayScene>(&this->state)->render(game);
			break;
	}
}
