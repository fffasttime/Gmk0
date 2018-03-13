#include "Search.h"
#include "Game.h"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <random>

//sum of weight should be 1.0
int randomSelect(const vector<float> &weight)
{
	static std::default_random_engine e(cfg_seed);
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
	static std::default_random_engine e(cfg_seed);
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

MCTS::MCTS(Board &_board, int _col, NN *_network, int _playouts):boardhash(_board)
{
	board = _board;
	nowcol = _col;
	network = _network;
	playouts = _playouts;
	tr = new Node[(playouts+2)*BLSIZE];
}

void MCTS::make_move(int move)
{
	ASSERT(move >= 0 && move < BSIZE*BSIZE);
	ASSERT(board(move) == 0);
	boardhash.update(move, board[move], nowcol);
	board[move] = nowcol;
	nowcol = nowcol % 2 + 1;
}

void MCTS::unmake_move(int move)
{
	ASSERT(move >= 0 && move < BSIZE*BSIZE);
	nowcol = nowcol % 2 + 1;
	ASSERT(board(move) == nowcol);
	boardhash.update(move, nowcol, 0);
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
	debug_s << "net win: " << result.v << '\n';
	if (add_noise)
		addNoise(root, 0.25f, 0.2f);
}

//LZ's selection, weaker than current
int MCTS::selection(int cur)
{
	if (tr[cur].ch.size() == 0)
		return cur;
	auto sum_policy = 0.0f;
	for (auto &ch : tr[cur].ch)
		if (tr[ch].cnt)
			sum_policy += tr[ch].policy;
	
	auto numerator = sqrtf((Val)tr[cur].ch.size());
	auto fpu = 0.3f * sqrtf(sum_policy);
	int maxc = 0; Val maxv = -FLOAT_INF;

	for (auto &ch : tr[cur].ch)
	{
		auto winrate = tr[ch].cnt == 0 ? (-tr[cur].sumv / tr[cur].cnt) - fpu : tr[ch].sumv / tr[ch].cnt;
		auto ucb = winrate + UCBC * tr[ch].policy * numerator / (1.0f + tr[ch].cnt);
		if (ucb > maxv)
		{
			maxv = ucb;
			maxc = ch;
		}
	}
	return maxc;
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
			//int maxp = selection(cur);
			
			int maxp = cur;
			for (auto &ch : tr[cur].ch)
			{
				Val ucb;
				if (tr[ch].cnt == 0)
					ucb = (-tr[cur].sumv / tr[cur].cnt + 1.0f) / 1.1f - 1.0f + UCBC * tr[ch].policy*sqrtf((Val)tr[cur].cnt);
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
	debug_s << "mc win: " << -tr[root].sumv / tr[root].cnt<<'\n' << counter << '\n';
	logRefrsh();
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
	if (!tr[cur].ch.empty())
		tr[cur].ch.clear();
	for (int i = 0; i < BLSIZE; i++)
		if (board[i]==0) //for valid
		{
			tr[cur].ch.push_back(trcnt);
			tr[trcnt].sumv = 0;
			tr[trcnt].cnt = 0;
			tr[trcnt].policy = output.p[i];
			tr[trcnt].move = i;
			tr[trcnt].fa = cur;
			trcnt++;
		}
}

void MCTS::simulation_back(int cur)
{
	Val val;
	if (tr[cur].cnt == 0 && judgeWin(board) == 0 && board.count() < BLSIZE) //isn't end node
	{
		auto result = getEvaluation(board, nowcol, network, use_transform, tr[cur].move);
		val = -result.v;
		expand(cur, result);
		auto &it = hash_table.find(boardhash());
		if (it != hash_table.end())
			counter++;
		else
			hash_table[boardhash()] = cur;
	}
	else
	{
		val = 1; 
		if (board.count() == BLSIZE)
			val = 0;
	}
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

int MCTS::solvePolicy(Val te, BoardWeight &policy, float &winrate)
{
	solve(policy);
	winrate = -tr[0].sumv / tr[0].cnt;
	auto tpolicy = policy;

	{
		Val sum = 0;
		for (int i = 0; i < BLSIZE; i++)
		{
			policy[i] = powf(policy[i], 1.0f / 0.7f);
			sum += policy[i];
		}
		for (int i = 0; i < BLSIZE; i++)
			policy[i] /= sum;
	}

	//Tempearture
	if (te < 0.2) //tau-->0
	{
		int maxc; Val maxv=-FLOAT_INF;
		for (int i = 0; i < BLSIZE; i++)
			if (maxv < tpolicy[i])
			{
				maxc = i;
				maxv = tpolicy[i];
			}
		for (int i = 0; i < BLSIZE; i++) tpolicy[i] = 0;
		tpolicy[maxc] = 1;
		return maxc;
	}
	else
	{
		Val sum = 0;
		for (int i = 0; i < BLSIZE; i++)
		{
			tpolicy[i] = powf(tpolicy[i], 1.0f / te);
			sum += tpolicy[i];
		}
		for (int i = 0; i < BLSIZE; i++)
			tpolicy[i] /= sum;
		return randomSelect(tpolicy, BLSIZE);
	}
}

Coord Player::randomOpening(Board gameboard)
{
	if (gameboard.count() == 0)
	{
		return { 4+rand()%7, 4 + rand() % 7 };
	}
	else if (gameboard.count() == 1)
	{
		int first;
		for (int i = 0; i < BLSIZE; i++)
			if (gameboard[i] == 1)
				first = i;
		int dir = rand() % 8;
		return { cx[dir] + first / BSIZE, cy[dir] + first % BSIZE };
	}
	else
	{
		int first;
		for (int i = 0; i < BLSIZE; i++)
			if (gameboard[i] == 1)
				first = i;
		int x = first / BSIZE, y = first%BSIZE;
		BoardWeight po;
		po.clear();
		float sum = 0;
		for (int i = x-3; i <= x+3; i++)
			for (int j = y-3; j <= y+3; j++)
			{
				if (gameboard(i, j) == 0)
					if (i >= x-1 && i <= x+1 && j >= y-1 && j <= y+1)
						po(i, j) = 1, sum+=1;
					else
						po(i, j) = 1, sum+=1;
			}
		for (int i = 0; i < BLSIZE; i++)
			po[i] /= sum;
		return randomSelect(po, BLSIZE);
	}
}

Coord Player::run(Board &gameboard, int nowcol)
{
	MCTS mcts(gameboard, nowcol, &network, cfg_playouts);
	mcts.add_noise = cfg_add_noise;
	mcts.UCBC = cfg_puct;
	mcts.use_transform = cfg_use_transform;
	if (gameboard.count() < 3)
	{
		mcts.solvePolicy(cfg_temprature1, policy, winrate); //keep policy
		return randomOpening(gameboard);
	}
	if (gameboard.count()>=cfg_temprature_moves)
		return Coord(mcts.solvePolicy(cfg_temprature2, policy, winrate));
	else
		return Coord(mcts.solvePolicy(cfg_temprature1, policy, winrate));
}
