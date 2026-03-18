#pragma once

struct Game;

class MainMenuScene {
public:
	void update(Game* game);
	void render(const Game& game) const;
};
