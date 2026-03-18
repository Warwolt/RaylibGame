#pragma once

struct Game;

class GameplayScene {
public:
	void update(Game* game);
	void render(const Game& game) const;
};
