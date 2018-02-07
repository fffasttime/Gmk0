#pragma once
#include "Board.h"
#include "Common.h"
#include "Search.h"

int judgeWin(Board &board);

class Game
{
private:
	int gamestep;
	int nowcol;
	Board gameboard;
	void make_move(Coord pos);
	void reset();
public:
	void runGame(Player &player1, Player &player2);
	void runGame_selfplay(Player &player);
	void runRecord(const std::vector<int> &moves);
	void runFromFile(string filename);
	void selfplay(Player &player);
};