// MyGomoku.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "RunPython.h"

int main()
{
	PythonInstance pi;
	pi.callScirpt("Estimate");

	//minit();
	//while (1)
	{
		//runGame();
	}
	//runFromFile("gomoku.log");
	//mexit();
	getchar();
	return 0;
}
