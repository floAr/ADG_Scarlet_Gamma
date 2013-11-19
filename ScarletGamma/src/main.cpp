#include "Game.hpp"

Game* g_Game;

int main(int argc, const char* argv[])
{
	// Create game object
	g_Game = new Game();

	// Run the game
	g_Game->Init();
	g_Game->Run();
	g_Game->CleanUp();

	// Delete game objects and return
	delete g_Game;
	return 0;
}