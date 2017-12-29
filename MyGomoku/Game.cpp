#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include <vector>
#include "GameData.h"
#include <Windows.h>
using std::vector;

Board emptygameboard = {
	{
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,
		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  } };
Board gameboard;

void swap3(Board &board)
{
	for (int i = 0; i < BSIZE; i++)
		for (int j = 0; j < BSIZE; j++)
			if (board(i, j))
				board(i, j) = othercol(board(i, j));
}

int judgeWin(Board &board)
{
	for (int i = 0; i < BSIZE; i++)
		for (int j = 0; j < BSIZE; j++)
			if (board(i,j))
			{
				int col = board(i,j);
				for (int d = 0; d < 4; d++)
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

int gamestep;

void setPiece(Board& board, Coord &s, int col)
{
	board(s) = col;
}

int nowcol = 1;
int othercol(int col)
{
	return (col & 1) + 1;
}

void runGame()
{
	DataSeries<EposideData> datas;
	Board gameboard = emptygameboard;
	gamestep = 0;
	print(gameboard);
	vector<int> history;
	while (gamestep < BLSIZE)
	{
		Coord c;
		if (nowcol == 1) c = getPlayerPos(gameboard);
		else c = run(gameboard, nowcol, history.size()>0?Coord(history[history.size() - 1]):Coord(-1,-1));
		/*
		if (gamestep == 3 && c.x == 14 && c.y == 14)
		{
			swap3(gameboard);
			gamestep--;
		}
		else*/
		setPiece(gameboard, c, nowcol);
		history.push_back(c.p());
		print(gameboard);
		if (judgeWin(gameboard))
		{
			if (nowcol == C_W)
				printf("\nWhite win!");
			else
				printf("\nBlack win!");
			break;
		}
		nowcol = othercol(nowcol);
		//Sleep(500);
		gamestep++;
	}
	printf("\nDRAW!");
	datas.dump(EposideData(history, nowcol));
	datas.writeString("gomoku.log");
}

void runRecord(const vector<int> &moves)
{
	Board gameboard = emptygameboard;
	gamestep = 0;
	print(gameboard);
	for (auto move : moves)
	{
		Coord c(move);
		setPiece(gameboard, c, nowcol);

		print(gameboard);
		Sleep(500);

		nowcol = othercol(nowcol);
		gamestep++;
	}
}

void runFromFile(string filename)
{
	DataSeries<EposideData> datas;
	datas.readString(filename);
	for (auto &data : datas.datas)
	{
		runRecord(data.moves);
	}
}
