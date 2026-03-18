#include "game/scene/scene.h"

#include "game/game.h"

#include <functional>
#include <raylib.h>

void MainMenuScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->should_quit = true;
	}
}

void MainMenuScene::render(const Game& game) const {
	const int font_size = 32;
	const char* text = "Main Menu";
	const int text_width = Raylib_MeasureText(text, font_size);
	const int pos_x = (game.window.width() - text_width) / 2;
	const int pos_y = (game.window.height() - font_size) / 2;
	Raylib_DrawText(text, pos_x, pos_y, font_size, WHITE);
	Raylib_ClearBackground(Color { 0, 127, 127, 255 });
}

void GameplayScene::update(Game* game) {
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->should_quit = true;
	}
}

void GameplayScene::render(const Game& game) const {
	const int font_size = 32;
	const char* text = "Gameplay";
	const int text_width = Raylib_MeasureText(text, font_size);
	const int pos_x = (game.window.width() - text_width) / 2;
	const int pos_y = (game.window.height() - font_size) / 2;
	Raylib_ClearBackground(Color { 0, 127, 0, 255 });
	Raylib_DrawText(text, pos_x, pos_y, font_size, WHITE);
}

struct SceneVTable {
	std::function<void(Game*)> update;
	std::function<void(const Game&)> render;
};

SceneVTable scene_vtable(Scene* scene) {
	switch (scene->id) {
		case SceneID::MainMenu: {
			MainMenuScene* main_menu = std::get_if<MainMenuScene>(&scene->state);
			return SceneVTable {
				.update = [main_menu](Game* game) { main_menu->update(game); },
				.render = [main_menu](const Game& game) { main_menu->render(game); },
			};
		} break;

		case SceneID::Gameplay: {
			GameplayScene* gameplay = std::get_if<GameplayScene>(&scene->state);
			return SceneVTable {
				.update = [gameplay](Game* game) { gameplay->update(game); },
				.render = [gameplay](const Game& game) { gameplay->render(game); },
			};
		} break;
	}
	return SceneVTable {};
}

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
	SceneVTable vtable = scene_vtable(this);
	vtable.update(game);
}

void Scene::render(const Game& game) const {
	SceneVTable vtable = scene_vtable(const_cast<Scene*>(this));
	vtable.render(game);
}
