#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<list>
#include<string>
#include<ctime>
#include<vector> 
#include<sstream>
#include<fstream>

#define WIN_CON
#include<windows.h>
using namespace std;

int judgeWin(Board &board)
{
	for (int i = 0;i<BSIZE;i++)
		for (int j = 0;j<BSIZE;j++)
			if (board.m[i][j])
			{
				int col = board.m[i][j];
				for (int d = 0;d<4;d++)
				{
					int dx = i, dy = j, cnt = 0;
					do
					{
						dx += cx[d]; dy += cy[d];
						cnt++;
					} while (inBorder({ dx,dy }) && board(dx, dy) == col);
					if (cnt >= 5)
						return col;
				}
			}
	return 0;
}

const int C_E = 0, C_B = 1, C_W = 2;

int gamestep;

void setPiece(Board& board, Crood &s, int col)
{
	board(s) = col;
}

int nowcol = 1;
int othercol(int col)
{
	return (col & 1) + 1;
}


int main()
{
	minit();
	runGame();
	mexit();
	return 0;
}

