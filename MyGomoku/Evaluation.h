#pragma once
#include "Utils.h"
#include "Board.h"
#include <array>

struct RawInput
{
	//color w, color b
	float feature[BSIZE][BSIZE][2];
	RawInput(Board &board);
};

struct RawOutput
{
	float v;
	float p[BSIZE][BSIZE];
};

