#include "Evaluation.h"

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

RawOutput getEvaluation(Board board, int col, NN *network, bool use_transform)
{
	int trans;
	if (use_transform)
		trans = rand() % 8;
	else
		trans = 0;
	boardTransform(trans, board);
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
	RawOutput output;
	for (int i = 0; i<BLSIZE; i++)
		output.p[i] = ret.first[i];
	boardTransform(trans + 8, output.p);
	output.v = ret.second;
	return output;
}
