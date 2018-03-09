#include "Board.h"
#include <random>

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

int posTransform(int mode, int p)
{
	return transform_table[mode][p];
}

BoardArray<unsigned long long> zobirst_table[3];

void initZobristTable()
{
	std::mt19937 e(cfg_seed);
	std::uniform_int<unsigned long long> u(0, 1ull<<63);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < BLSIZE; j++)
		{
			zobirst_table[i][j] = u(e);
		}
}

BoardHasher::BoardHasher(Board &board)
{
	num = 0;
	for (int i = 0; i < BLSIZE; i++)
		num ^= zobirst_table[board[i]][i];
}

void BoardHasher::update(int pos, int old, int target)
{
	num ^= zobirst_table[old][pos];
	num ^= zobirst_table[target][pos];
}
