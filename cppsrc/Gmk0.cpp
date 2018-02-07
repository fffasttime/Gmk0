// Gmk0.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include "NN/nn_cpp.h"

int main()
{
	Player player1("NN/I17.txt");

	minit();
	initTransformTable();
	while (1)
	{
		runGame(player1, player1);
		system("pause");
	}
	mexit();

	getchar();
	return 0;
}
