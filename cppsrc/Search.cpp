#include "Search.h"
#include "Game.h"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <random>

//sum of weight should be 1.0
int randomSelect(const vector<float> &weight)
{
	float s = 1.0f;
	std::uniform_real<float>(0.0f, 1.0f);
	for (int i = 0; i < weight.size(); i++)
	{
		s -= weight[i];
		if (s < 0) return i;
	}
	ASSERT(false);
	return 0;
}
int randomSelect(BoardWeight weight, int count)
{
	float s = 1.0f;
	std::uniform_real<float>(0.0f, 1.0f);
	for (int i = 0; i < count; i++)
	{
		s -= weight[i];
		if (s < 0) return i;
	}
	ASSERT(false);
	return 0;
}

MCTS::MCTS(const Board &_board, int _col, int lastmove)
{
	tr = new Node[300000];
	board = _board;
	nowcol = _col;
	tr[root].cnt = 0;
	tr[root].fa = -1;
	tr[root].sumv = 0;
	tr[root].move = lastmove;
}

void MCTS::make_move(int move)
{
	ASSERT(move >= 0 && move < BSIZE*BSIZE);
	ASSERT(board(move) == 0);
	board(move) = nowcol;
	nowcol = nowcol % 2 + 1;
}

void MCTS::unmake_move(int move)
{
	ASSERT(move >= 0 && move < BSIZE*BSIZE);
	nowcol = nowcol % 2 + 1;
	ASSERT(board(move) == nowcol);
	board(move) = 0;
}

void MCTS::solve(BoardWeight &result)
{
	int rcnt = 1;
	for (int i = 0; i < rcnt; i++)
	{
		int cur = root;
		while (1)
		{
			Val maxv = -FLOAT_INF;
			int maxp = cur;
			if (tr[cur].sumv < -1) break;
			for (auto &ch : tr[cur].ch)
			{
				Val ucb;
				if (tr[ch].cnt == 0)
					ucb = UCBC*tr[ch].policy*sqrtf((Val)tr[cur].cnt);
				else
					ucb = tr[ch].sumv / tr[ch].cnt + UCBC*tr[ch].policy*sqrtf((Val)tr[cur].cnt) / (1 + tr[ch].cnt);
				if (ucb > maxv)
				{
					maxv = ucb;
					maxp = ch;
				}
			}
			//leaf node
			if (maxp == cur) break;
			//forward search
			cur = maxp;
			make_move(tr[cur].move);
		}
		//simulation & backpropagation
		simulation_back(cur);
	}

	result.clear();
	for (auto ch : tr[root].ch)
	{
		result[tr[ch].move] = (Val)tr[ch].cnt;
		//std::cout << tr[ch].move << ' ' << tr[ch].cnt << ' ' << tr[ch].sumv / tr[ch].cnt << '\n';
	}
}
#if 0
Val MCTS::getEndVal(const Board &map, int col)
{
	int cnt[3];
	//map.countPiece(cnt); int co1 = col_f(col);
	//if (cnt[col]>cnt[co1]) return 1.0f;
	//else if (cnt[col] == cnt[co1]) return 0.5f;
	//else return 0.0f;
	return (tanh((cnt[col] - cnt[col]) / 16.0) + 1) / 2;
}
#endif
Val MCTS::getValue()
{
	int result = judgeWin(board);
	if (result)
		return result == nowcol ? -1.0f : 1.0f;
	return 0.0f;
}

void MCTS::getPolicy(int cur, BoardWeight &result)
{
	int x = tr[cur].move / 15, y = tr[cur].move % 15;
	int availcnt = 0;
	for (int i = x - 3; i <= x + 3; i++)
		for (int j = y - 3; j <= y + 3; j++)
			if (inBorder(i, j) && board(i, j) == 0)
				availcnt++;
	for (int i = 0; i < BLSIZE; i++)
		result[i] = 0;
	for (int i = x - 3; i <= x + 3; i++)
		for (int j = y - 3; j <= y + 3; j++)
			if (inBorder(i, j) && board(i, j) == 0)
				result[i * 15 + j] = 1.0f / availcnt;
}

void MCTS::expand(int cur,const RawOutput &output)
{
	//BoardWeight prob;
	//getPolicy(cur, prob);
	for (int i = 0; i < BLSIZE; i++)
		if (output.p[i] > 1e-3)
		{
			tr[cur].ch.push_back(trcnt);
			tr[trcnt].sumv = 0;
			tr[trcnt].cnt = 0;
			tr[trcnt].policy = output.p[i];
			tr[trcnt].move = i;
			if (!tr[trcnt].ch.empty())
				tr[trcnt].ch.clear();
			tr[trcnt].fa = cur;
			trcnt++;
		}
}

void MCTS::simulation_back(int cur)
{
	//isn't end node
	Val val;
	if (tr[cur].cnt == 0)
	{
		//val = getValue();
		RawOutput result;
		getEvaluation(board, result);
		val = result.v;
		if (judgeWin(board) == 0)
			expand(cur, result);

	}
	else
		val = getValue();
	tr[cur].sumv += val;
	tr[cur].cnt++;
	//back
	while (cur > 0)
	{
		unmake_move(tr[cur].move);
		cur = tr[cur].fa;
		val = - val;
		tr[cur].sumv += val;
		tr[cur].cnt++;
	}
}

void MCTS::solvePolicy(Val te)
{
	BoardWeight v;
	solve(v);
	int dec;
	//Tempearture change
	//for ulitmate
	if (te < 0.1)
	{
		int maxc; Val maxv=-FLOAT_INF;
		for (int i = 0; i < BLSIZE; i++)
			if (maxv < v[i])
			{
				maxc = i;
				maxv = v[i];
			}
		for (int i = 0; i < BLSIZE; i++) v[i] = 0;
		v[maxc] = 1;
		dec = maxc;
	}
	else
	{
		Val sum = 0;
		for (int i = 0; i < BLSIZE; i++)
		{
			v[i] = powf(v[i], te);
			sum += v[i];
		}
		for (int i = 0; i < BLSIZE; i++)
			v[i] /= sum;
		dec = randomSelect(v, BLSIZE);
	}
	
}

Coord run(const Board &gameboard, int nowcol, Coord lastmove)
{
#if 0
	int r = rand() % 225;
	return { r / 15,r % 15 };
#else
	if (!inBorder(lastmove)) return { 7,7 };
	MCTS mcts(gameboard,nowcol,lastmove.p());
	BoardWeight result;
	mcts.solve(result);
	//argmax
	float maxc = 0; int maxp = 0;
	for (int i = 0; i < BLSIZE; i++)
		if (result[i] > maxc)
		{
			maxc = result[i];
			maxp = i;
		}
	return Coord(maxp);
#endif
}
