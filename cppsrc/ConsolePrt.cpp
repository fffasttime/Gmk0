#include "ConsolePrt.h"

#ifdef WIN_CON
#include <Windows.h>

HANDLE hOut, hIn;

void minit()
{
	HANDLE consolehwnd;
	consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleTextAttribute(consolehwnd, 127 + 128 - 15);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	//SMALL_RECT rc={0,0,winsize.X,winsize.Y};
	//SetConsoleScreenBufferSize(hOut, winsize);
	//SetConsoleWindowInfo(hOut, TRUE, &rc);
}

void mexit()
{
	CloseHandle(hOut);
	CloseHandle(hIn);
}

Coord getCurClick()
{
	//CONSOLE_SCREEN_BUFFER_INFO bInfo;
	INPUT_RECORD    mouseRec;
	DWORD           res;
	COORD           crPos;
	while (1)
	{
		ReadConsoleInput(hIn, &mouseRec, 1, &res);
		if (mouseRec.EventType == MOUSE_EVENT)
		{
			crPos = mouseRec.Event.MouseEvent.dwMousePosition;
			switch (mouseRec.Event.MouseEvent.dwButtonState)
			{
			case FROM_LEFT_1ST_BUTTON_PRESSED:
				return{ crPos.Y, crPos.X };
			}
		}
	}
}

void gotoXY(short x, short y)
{
	SetConsoleCursorPosition(hOut, { x, y });
}

#endif

Coord MlocToPloc(const Coord &p)
{
	if (p.y >= 2 * BSIZE || p.x >= BSIZE) return{ -1,-1 };
	return{ p.x,p.y / 2 };
}

Coord getPlayerPos(Board &gameboard)
{
	auto sp = MlocToPloc(getCurClick());
	while (!inBorder(sp) || gameboard(sp))
	{
		sp = MlocToPloc(getCurClick());
	}
	return sp;
}

void print(Board &gameboard)
{
	gotoXY(0, 0);
	for (int i = 0; i<BSIZE; i++)
	{
		for (int j = 0; j<BSIZE; j++)
		{
			if (gameboard(i, j) == C_E)
				printf("©à");
			else if (gameboard(i, j) == C_W)
				printf("¡ð");
			else
				printf("¡ñ");
		}/*
		for (int j = 0; j<BSIZE; j++)
		{
			if (gameboard(i, j) == C_E)
				fout << "©à";
			else if (gameboard(i, j) == C_W)
				fout << "¡ð";
			else
				fout << "¡ñ";
		}*/
		printf("\n");
		//fout << "\n";
	}

	logRefrsh();
}
