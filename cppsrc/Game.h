#pragma once
#include "Board.h"
#include "Utils.h"

int judgeWin(Board &board);
void setPiece(Board& board, Coord &s, int col);
int othercol(int col);

void runGame();
void runRecord(const std::vector<int> &moves);
void runFromFile(string filename);