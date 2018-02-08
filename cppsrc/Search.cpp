#include "Search.h"
#include "Game.h"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <random>

//sum of weight should be 1.0
int randomSelect(const vector<float> &weight)
{
	static std::default_random_engine e;
	static std::uniform_real<float> u(0.0f, 1.0f);
	float s = u(e);
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
	static std::default_random_engine e;
	static std::uniform_real<float> u(0.0f, 1.0f);
	float s = u(e);
	for (int i = 0; i < count; i++)
	{
		s -= weight[i];
		if (s < 0) return i;
	}
	ASSERT(false);
	return 0;
}

void MCTS::addNoise(int cur, Val epsilon, Val alpha) 
{
	static std::default_random_engine e;
	std::vector<Val> dirichlet;
	std::gamma_distribution<float> gamma(alpha, 1.0f);
	Val sum = 0;
	for (int i = 0; i < BLSIZE; i++) 
	{
		dirichlet.push_back(gamma(e));
		sum += dirichlet[i];
	}

	for (size_t i = 0; i < tr[cur].ch.size(); i++)
	{
		int ch = tr[cur].ch[i];
		tr[ch].policy = (1-epsilon) * tr[ch].policy + epsilon * dirichlet[i] / sum;
	}
}

MCTS::MCTS(const Board &_board, int _col, NN *_network, int _playouts)
{
	board = _board;
	nowcol = _col;
	network = _network;
	playouts = _playouts;
	tr = new Node[playouts*BLSIZE];
}

void MCTS::make_move(int move)
{
	ASSERT(move >= 0 && move < BSIZE*BSIZE);
	ASSERT(board(move) == 0);
	board[move] = nowcol;
	nowcol = nowcol % 2 + 1;
}

void MCTS::unmake_move(int move)
{
	ASSERT(move >= 0 && move < BSIZE*BSIZE);
	nowcol = nowcol % 2 + 1;
	ASSERT(board(move) == nowcol);
	board[move] = 0;
}

void MCTS::createRoot()
{
	tr[root].cnt = 1;
	tr[root].fa = -1;
	tr[root].move = -1;
	auto result = getEvaluation(board, nowcol, network, use_transform);
	tr[root].sumv = -result.v;
	expand(root, result);
	//addNoise(root, 0.25f, 0.03f);
}

void MCTS::solve(BoardWeight &result)
{
	createRoot();
	for (int i = 0; i < playouts; i++)
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
					ucb = tr[cur].sumv / tr[cur].cnt / 1.3f + UCBC * tr[ch].policy*sqrtf((Val)tr[cur].cnt);
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

Val MCTS::getValue()
{
	int result = judgeWin(board);
	if (result)
		return result == nowcol ? -1.0f : 1.0f;
	return 0.0f;
}

void MCTS::expand(int cur,RawOutput &output)
{
	for (int i = 0; i < BLSIZE; i++)
		if (board[i]==0) //for valid
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
	Val val;
	if (tr[cur].cnt == 0 && judgeWin(board) == 0) //isn't end node
	{
		auto result = getEvaluation(board, nowcol, network, use_transform);
		val = -result.v;
		expand(cur, result);
	}
	else
		val = 1;
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

int MCTS::solvePolicy(Val te, BoardWeight &policy)
{
	solve(policy);
	//Tempearture
	if (te < 0.2) //t-->0
	{
		int maxc; Val maxv=-FLOAT_INF;
		for (int i = 0; i < BLSIZE; i++)
			if (maxv < policy[i])
			{
				maxc = i;
				maxv = policy[i];
			}
		for (int i = 0; i < BLSIZE; i++) policy[i] = 0;
		policy[maxc] = 1;
		return maxc;
	}
	else
	{
		Val sum = 0;
		for (int i = 0; i < BLSIZE; i++)
		{
			policy[i] = powf(policy[i], 1.0f / te);
			sum += policy[i];
		}
		for (int i = 0; i < BLSIZE; i++)
			policy[i] /= sum;
		return randomSelect(policy, BLSIZE);
	}
}

Coord Player::run(const Board &gameboard, int nowcol)
{
	MCTS mcts(gameboard, nowcol, &network, cfg_playouts);
	mcts.add_noise = cfg_add_noise;
	mcts.UCBC = cfg_puct;
	mcts.use_transform = cfg_use_transform;
	if (gameboard.count()>=cfg_temprature_moves)
		return Coord(mcts.solvePolicy(cfg_temprature2, policy));
	else
		return Coord(mcts.solvePolicy(cfg_temprature1, policy));
}
