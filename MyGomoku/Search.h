#pragma once

#include "Utils.h"
#include "Board.h"
#include <vector>
using std::vector;

typedef float Val;
class MCTS
{
private:
	const Val UCBC = 1.0f;
	struct Node
	{
		Val sumv,policy;
		int cnt; int fa;
		vector<int> ch;
		int move;
		void print()
		{
			//fout << "rate:" << score / cnt << " cnt:" << cnt << " " << Coord(p) << " ch:";
			//for (auto it : ch) fout << it << ' ';
			//fout << '\n';
		}
	};
	Node *tr;
	const int root = 0;
	int trcnt = 1, viscnt = 0;
	Board board;
	int nowcol;
private:
	void make_move(int move);
	void unmake_move(int move);
public:
	MCTS();
	void solve(const Board &_board, int _col, int(&result)[BSIZE*BSIZE], int lastmove);
	//Val getEndVal();
	Val getValue();
	void getPolicy(int cur, Val result[BLSIZE]);
	void expand(int cur);
	void simulation_back(int cur);
	~MCTS()
	{
		delete[] tr;
	}
};

Coord run(Board gameboard, int nowcol, Coord lastmove);
