// Gmk0.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include "NN/nn_cpp.h"

int main()
{
	Player player1("NN/I17.txt");
	Game game;

	initTransformTable();
	minit();
	game.selfplay(player1);
	mexit();

	getchar();
	return 0;
}
