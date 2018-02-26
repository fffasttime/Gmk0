#pragma once
#include "Common.h"

bool inBorder(Coord a);
bool inBorder(int x, int y);

template<typename T>
struct BoardArray
{
	T m[BLSIZE];
	T& operator()(Coord o)
	{
		return m[o.p()];
	}
	T& operator()(int x, int y)
	{
		return m[x * BSIZE + y];
	}
	T& operator[](int p)
	{
		return m[p];
	}
	void debug() const;
	void clear();
	int count() const;
};


typedef BoardArray<int> Board;
typedef BoardArray<float> BoardWeight;

extern Board transform_table[16];

void initTransformTable();

template<typename T>
void boardTransform(int mode, BoardArray<T> &board)
{
	ASSERT(mode >= 0 && mode < 16);
	auto copy = board;
	for (int i = 0; i < BLSIZE; i++)
		board[i] = copy[transform_table[mode][i]];
}

int posTransform(int mode, int p);
//*

//*/

const int cx[8] = { -1,-1,-1,0,0,1,1,1 };
const int cy[8] = { -1,0,1,-1,1,-1,0,1 };

