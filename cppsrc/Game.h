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
	void printWinner(int z);
public:
	void runGame(Player &player1, Player &player2);
	void runGame_selfplay(Player &player);
	void runRecord(const std::vector<int> &moves);
	void runFromFile(string filename);
	void selfplay(Player &player);
	void match(Player & player1, Player & player2);
	string output_file = "selfplaydata.txt";
	int show_mode = 0;
};
