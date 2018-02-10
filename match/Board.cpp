#include "Board.h"

bool inBorder(Coord a)
{
	return a.x >= 0 && a.y >= 0 && a.x < BSIZE && a.y < BSIZE;
}

bool inBorder(int x, int y)
{
	return x >= 0 && y >= 0 && x < BSIZE && y < BSIZE;
}
template <>
void BoardArray<int>::clear()
{
	memset(m, 0, sizeof(m));
}

template <>
int BoardArray<int>::count() const
{
	int cnt = 0;
	for (int i = 0; i < BLSIZE; i++)
		if (m[i])
			cnt++;
	return cnt;
}

template <>
void BoardArray<float>::clear()
{
	for (int i = 0; i < BLSIZE; i++)
		m[i] = 0;
}
void BoardArray<int>::debug() const
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

Board transform_table[16];

void initTransformTable()
{
	for (int i = 0; i < BLSIZE; i++)
		transform_table[0][i] = i;
	for (int i = 0; i < BSIZE; i++)
		for (int j = 0; j < BSIZE; j++)
			transform_table[1][i*BSIZE + j] = j*BSIZE + BSIZE - i - 1;
	for (int i = 0; i < BSIZE; i++)
		for (int j = 0; j < BSIZE; j++)
			transform_table[4][i*BSIZE + j] = i*BSIZE + BSIZE - j - 1;
	transform_table[2] = transform_table[1];
	boardTransform(1, transform_table[2]);
	transform_table[3] = transform_table[2];
	boardTransform(1, transform_table[3]);
	transform_table[5] = transform_table[4];
	boardTransform(1, transform_table[5]);
	transform_table[6] = transform_table[5];
	boardTransform(1, transform_table[6]);
	transform_table[7] = transform_table[6];
	boardTransform(1, transform_table[7]);

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < BLSIZE; j++)
			transform_table[i + 8][transform_table[i][j]] = j;
}
