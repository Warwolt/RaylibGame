#pragma once

struct Game;

class MainMenuScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;
};
