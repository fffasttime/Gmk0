#pragma once
#include "Utils.h"

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
	void debug();
	void clear();

};

typedef BoardArray<int> Board;
typedef BoardArray<float> BoardWeight;

//*

//*/

const int cx[8] = { -1,-1,-1,0,0,1,1,1 };
const int cy[8] = { -1,0,1,-1,1,-1,0,1 };

