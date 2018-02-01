#pragma once

#include "Utils.h"
#include "Board.h"
#include "Evaluation.h"
#include <vector>
using std::vector;

typedef float Val;
int randomSelect(const vector<float> &weight);
int randomSelect(BoardWeight weight, int count);

class MCTS
{
private:

	const Val UCBC = 2.8f;
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
	MCTS(const Board &_board, int _col, int lastmove);
	void solve(BoardWeight &result);
	void solvePolicy(Val te);
	//Val getEndVal();
	Val getValue();
	void getPolicy(int cur, BoardWeight &result);
	void expand(int cur,const RawOutput &output);
	void simulation_back(int cur);
	~MCTS()
	{
		delete[] tr;
	}
};

Coord run(const Board &gameboard, int nowcol, Coord lastmove);
