#include "Board.h"

bool inBorder(Coord a)
{
	return a.x >= 0 && a.y >= 0 && a.x < BSIZE && a.y < BSIZE;
}

bool inBorder(int x, int y)
{
	return x >= 0 && y >= 0 && x < BSIZE && y < BSIZE;
}

void BoardArray<int>::clear()
{
	memset(m, 0, sizeof(m));
}

void BoardArray<float>::clear()
{
	for (int i = 0; i < BLSIZE; i++)
		m[i] = 0;
}
void BoardArray<int>::debug()
{/*
	for (int i = 0; i<BSIZE; i++)
	{
		for (int j = 0; j<BSIZE; j++)
			fout << " " << m[i][j];
		fout << '\n';
	}
	fout << '\n';
	logRefrsh();*/
}
