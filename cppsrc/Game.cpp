#include "Game.h"
#include "ConsolePrt.h"
#include "Search.h"
#include <vector>
#include "GameData.h"
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

void swap3(Board &board)
{
	for (int i = 0; i < BLSIZE; i++)
		if (board[i])
			board[i] = board[i] % 2 + 1;
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

void Game::make_move(Coord pos)
{
	gameboard(pos) = nowcol;
	nowcol = nowcol % 2 + 1;
}

void Game::reset()
{
	gameboard = emptygameboard;
	gamestep = 0;
	nowcol = 1;
}

void Game::printWinner(int z)
{
	if (z == 1) 
		std::cout << "Black win!";
	else if (z==2) 
		std::cout << "White win!";
	else 
		std::cout << "Draw!";
	std::cout << std::endl;
}

void Game::runGame(Player &player1, Player &player2)
{
	reset();
	vector<int> history;
	if (show_mode == 1) print(gameboard);
	while (gamestep < BLSIZE)
	{
		Coord c;
		if (nowcol == 1)
			c = player1.run(gameboard, nowcol);
		else
			c = player2.run(gameboard, nowcol);
		make_move(c);
		history.push_back(c.p());
		if (show_mode == 1) print(gameboard);
		else if (show_mode == 0) std::cout << c.format() << ' ';
		if (judgeWin(gameboard))
			break;
		gamestep++;
	}
	int winner = nowcol % 2 + 1;
	if (gamestep == BLSIZE) winner = 0;
	printWinner(winner);
}

void Game::runGame_selfplay(Player &player)
{
	reset();
	vector<int> history;
	vector<BoardWeight> policy;
	if (show_mode==1) print(gameboard);
	while (gamestep < BLSIZE)
	{
		Coord c = player.run(gameboard, nowcol);
		make_move(c);
		history.push_back(c.p());
		policy.push_back(player.getlastPolicy());
		if (show_mode == 1) print(gameboard);
		else if (show_mode == 0) std::cout << c.format() <<' ';
		if (judgeWin(gameboard))
			break;
		gamestep++;
	}
	int winner = nowcol % 2 + 1;
	if (gamestep == BLSIZE) winner = 0;
	printWinner(winner);
	EposideTrainingData data(history, policy , winner);
	ofstream out(output_file, std::ios::app);
	data.writeString(out);
}

void Game::selfplay(Player &player)
{
	int play_counts = 4096;
	for (int i = 0; i < play_counts; i++)
	{
		std::cout << "game " << i << '\n';
		runGame_selfplay(player);
	}
}

void Game::match(Player &player1, Player &player2)
{
	int play_counts = 100;
	for (int i = 0; i < play_counts; i++)
	{
		std::cout << "game " << i << '\n';
		runGame(player1, player2);
	}
}

void Game::runRecord(const vector<int> &moves)
{
	reset();
	if (show_mode == 1) print(gameboard);
	for (auto move : moves)
	{
		Coord c(move);
		make_move(c);
		if (show_mode == 1) print(gameboard);
		else if (show_mode == 0) std::cout << c.format() << ' ';
		gamestep++;
	}
	int winner = nowcol % 2 + 1;
	if (gamestep == BLSIZE) winner = 0;
	printWinner(winner);
}

void Game::runFromFile(string filename)
{
	DataSeries<EposideData> datas;
	datas.readString(filename);
	for (auto &data : datas.datas)
	{
		runRecord(data.moves);
	}
}
