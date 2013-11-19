#include "Game.hpp"

Game* g_Game;

int main(int argc, const char* argv[])
{
	g_Game = new Game();
	//g_Game->run();
	delete g_Game;
	return 0;
}