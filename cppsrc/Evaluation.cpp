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

RawOutput getEvaluation(Board board, int col, NN *network, bool use_transform, int lastmove)
{
#if 0
	RawOutput output;
	for (int i = 0; i < BLSIZE; i++)
		output.p[i] = 1.0f / BLSIZE;
	output.v = 0;
	return output;
#else
	int trans;
	if (use_transform)
		trans = rand() % 8;
	else
		trans = 0;
	boardTransform(trans, board);
	Network::NNPlanes input;
	input.resize(2);
	for (int i = 0; i < BLSIZE; i++)
	{ 
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
	}
	auto ret = network->forward(input);
	double sum_policy = 0.0;
	//scale sum_policy to 1
	for (int i = 0; i<BLSIZE; i++)
		if (board[i]==0)
			sum_policy += ret.first[i];
	RawOutput output;
	if (sum_policy > 1e-10)
		for (int i = 0; i<BLSIZE; i++)
			output.p[i] = (float)(ret.first[i] / sum_policy);

	boardTransform(trans + 8, output.p);
	output.v = ret.second  * 2.0f - 1.0f;
	return output;
#endif
}
