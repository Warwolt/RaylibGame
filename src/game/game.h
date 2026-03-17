#pragma once

struct Game {
	bool should_quit = false;
};

extern "C" __declspec(dllexport) void Game_initialize(Game* game, int argc, char** argv);
extern "C" __declspec(dllexport) void Game_update(Game* game);
extern "C" __declspec(dllexport) void Game_render(const Game& game);
extern "C" __declspec(dllexport) void Game_shutdown(Game* game);
