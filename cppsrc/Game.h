#pragma once
#include "Board.h"
#include "Common.h"
#include "Search.h"

int judgeWin(Board &board);
void setPiece(Board& board, Coord &s, int col);
int othercol(int col);

void runGame(Player &player1, Player &player2);
void runRecord(const std::vector<int> &moves);
void runFromFile(string filename);