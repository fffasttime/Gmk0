// MyGomoku.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Evaluation.h"
#include "NN/nn_cpp.h"

int main()
{
	network = new NN(std::string("NN/I17.txt"));

	minit();
	while (1)
	{
		runGame();
		system("pause");
	}
	mexit();

	getchar();
	delete network;
	return 0;
}
