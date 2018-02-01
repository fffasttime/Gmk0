#pragma once
#include "Utils.h"
#include "Board.h"
#include <array>

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

int initNumpy();
void getEvaluation(Board &board, RawOutput &output);