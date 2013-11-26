#include "Game.hpp"
#include "UnitTest/UnitTests.hpp"

Game* g_Game;

int main(int argc, const char* argv[])
{
	// As long as in development...
	UnitTest::Run();

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