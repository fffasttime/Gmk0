// MyGomoku.cpp: 定义控制台应用程序的入口点。
//
#include "Game.h"
#include "ConsolePrt.h"
#include "Evaluation.h"
#include "NN/nn_cpp.h"

int main()
{
	network = new NN(std::string("NN/I17.txt"));
	Network::NNPlanes input;
	input.resize(INPUT_CHANNELS);
	for (int i = 0; i < 225; i++)
		input[0][i]=input[1][i] = 0;
	auto output = network->forward(input);
	float sum = 0;
	for (int i=0;i<225;i++)
	{
		sum += output.first[i];
		if (output.first[i] > 0.01) 
			std::cout << i << ' ' << output.first[i] << '\n';
	}
	std::cout << sum;
	system("pause");

	minit();
	while (1)
	{
		runGame();
	}
	mexit();

	getchar();
	delete network;
	return 0;
}
