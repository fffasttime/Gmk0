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

void getEvaluation(Board &board, RawOutput &output)
{
	output.v = 0;
	for (int i = 0; i < BLSIZE; i++)
		output.p[i] = 1.0f / BLSIZE;
}
