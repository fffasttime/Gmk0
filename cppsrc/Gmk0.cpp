// MyGomoku.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Evaluation.h"

int main()
{
	minit();
	while (1)
	{
		runGame();
	}
	mexit();
	getchar();
	return 0;
}
