#include "Evaluation.h"
#include "RunPython.h"

RawInput::RawInput(Board &board)
{
	for (int i = 0; i < BSIZE; i++)
		for (int j = 0; j < BSIZE; j++)
			if (board(i, j) == C_B)
			{
				feature[i][j][0] = 1; 
				feature[i][j][1] = 0;
			}
			else if (board(i,j)==C_W)
			{
				feature[i][j][0] = 0; 
				feature[i][j][1] = 1;
			}
			else
			{
				feature[i][j][0] = 0; 
				feature[i][j][1] = 0;
			}
}

void getEvaluation()
{
	auto py = PythonInstance::getInstance();
	auto fun = py->getFunc("forward");

}
