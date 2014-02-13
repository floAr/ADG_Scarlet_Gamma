#include "Game.hpp"
#include "UnitTest/UnitTests.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

Game* g_Game;

int main(int argc, const char* argv[])
{
#ifdef _DEBUG
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 182453 );

	// As long as in development...
	UnitTest::Run();
#endif

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