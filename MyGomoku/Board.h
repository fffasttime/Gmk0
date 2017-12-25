#pragma once
#include "Utils.h"

bool inBorder(Coord a);
bool inBorder(int x, int y);

struct Board
{
	int m[BSIZE][BSIZE];
	void clear();
	int& operator()(Coord o)
	{
		return m[o.x][o.y];
	}
	int& Board::operator()(int x, int y)
	{
		return m[x][y];
	}
	int& Board::operator()(int p)
	{
		return m[p/BSIZE][p%BSIZE];
	}
	void debug();

};

//*

//*/

const int cx[8] = { -1,-1,-1,0,0,1,1,1 };
const int cy[8] = { -1,0,1,-1,1,-1,0,1 };


