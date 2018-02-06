#include "Evaluation.h"

NN *network;

RawInput::RawInput(Board &board)
{
	for (int i = 0; i < BLSIZE; i++)
		if (board[i] == C_B)
		{
			feature[0][i] = 1; 
			feature[1][i] = 0;
		}
		else if (board[i]==C_W)
		{
			feature[0][i] = 0; 
			feature[1][i] = 1;
		}
		else
		{
			feature[0][i] = 0; 
			feature[1][i] = 0;
		}
}

void getEvaluation(Board &board, int col, RawOutput &output)
{
	Network::NNPlanes input;
	input.resize(2);
	for (int i = 0; i < BLSIZE; i++)
		if (board[i] == 0)
		{
			input[0][i] = 0;
			input[1][i] = 0;
		}
		else if (board[i] == col)
		{
			input[0][i] = 1;
			input[1][i] = 0;
		}
		else
		{
			input[0][i] = 0;
			input[1][i] = 1;
		}

	auto ret = network->forward(input);
	float sum = 0;
	for (int i = 0; i<BLSIZE; i++)
	{
		output.p[i] = ret.first[i];
	}
	output.v = ret.second;
}
