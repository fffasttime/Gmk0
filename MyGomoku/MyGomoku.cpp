// MyGomoku.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "RunPython.h"
#include "Evaluation.h"

int main()
{
	PythonInstance pi;
	pi.loadPackage("Estimate");
	pi.loadFunction("Estimate", "nptest");
//	testCall();
	getEvaluation();
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
