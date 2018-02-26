#pragma once
#include "Common.h"
#include "Board.h"
#include "NN/nn_cpp.h"
#include <array>

struct RawInput
{
	//color w, color b
	float feature[2][BLSIZE];
	RawInput(Board &board);
};

struct RawOutput
{
	BoardWeight p;
	float v;
};

RawOutput getEvaluation(Board board, int col, NN *network, bool use_transform = true, int lastmove=-1);