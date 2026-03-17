#pragma once

struct Game {
    bool should_quit;
};

__declspec(dllexport) void Game_initialize(Game* game, int argc, char** argv);
__declspec(dllexport) void Game_shutdown(Game* game);
__declspec(dllexport) void Game_update(Game* game);
__declspec(dllexport) void Game_render(const Game* game);
