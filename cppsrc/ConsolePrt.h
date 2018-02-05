#pragma once
#include "Common.h"
#include "Board.h"

#define WIN_CON

#ifdef WIN_CON

void minit();

void mexit();

Coord getCurClick(); 
Coord getPlayerPos(Board &gameboard);

void gotoXY(short x, short y);

#endif

Coord MlocToPloc(const Coord &p);
void print(Board &gameboard);