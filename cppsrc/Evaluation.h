#pragma once
#include "Common.h"
#include "Board.h"
#include "NN/nn_cpp.h"
#include <array>

extern NN *network;

struct RawInput
{
	//color w, color b
	float feature[2][BLSIZE];
	RawInput(Board &board);
};

struct RawOutput
{
	float p[BLSIZE];
	float v;
};

void getEvaluation(Board &board, int col, RawOutput &output);