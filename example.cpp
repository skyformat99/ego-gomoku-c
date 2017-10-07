#include "stdafx.h"
#include "pisqpipe.h"
#include <windows.h>
#include "game.h"
#include "io.h"

const char *infotext = "name=\"Random\", author=\"Petr Lastovicka\", version=\"3.2\", country=\"Czech Republic\", www=\"http://petr.lastovicka.sweb.cz\"";

#define MAX_BOARD 20
static Color** map;
static unsigned seed;
extern int boardSize;
extern int timeOut;

void brain_init()
{
	map = getEmptyMap();
	if (width < 5 || height < 5) {
		pipeOut("ERROR size of the board");
		return;
	}
	if (width > MAX_BOARD || height > MAX_BOARD) {
		pipeOut("ERROR Maximal board size is %d", MAX_BOARD);
		return;
	}
	boardSize = width;
	seed = start_time;
	pipeOut("OK");
}

void brain_restart()
{
	map = getEmptyMap();
	pipeOut("OK");
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && map[x][y] == NULL;
}

void brain_my(int x, int y)
{
	if (isFree(x, y)) {
		map[x][y] = BLACK;
	}
	else {
		pipeOut("ERROR my move [%d,%d]", x, y);
	}
}

void brain_opponents(int x, int y)
{
	if (isFree(x, y)) {
		map[x][y] = WHITE;
	}
	else {
		pipeOut("ERROR opponents's move [%d,%d]", x, y);
	}
}

void brain_block(int x, int y)
{
	if (isFree(x, y)) {
		map[x][y] = 3;
	}
	else {
		pipeOut("ERROR winning move [%d,%d]", x, y);
	}
}

int brain_takeback(int x, int y)
{
	if (x >= 0 && y >= 0 && x < width && y < height && map[x][y] != 0) {
		map[x][y] = 0;
		return 0;
	}
	return 2;
}

unsigned rnd(unsigned n)
{
	seed = seed * 367413989 + 174680251;
	return (unsigned)(UInt32x32To64(n, seed) >> 32);
}

void brain_turn()
{
	timeOut = 1000 * 1000;
	timeOut = min(info_timeout_turn, timeOut);
	int pointCount = 0;
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] != NULL) {
				pointCount++;
			}
		}
	}
	if (pointCount < 40) {
		long maxTime = info_time_left / (40 - pointCount);
		timeOut = min(maxTime, timeOut);
	}
	pipeOut("MESSAGE time limit %d", timeOut);
	gameResult result = search(BLACK, map);
	point p = result.result;
	pipeOut("MESSAGE [%d,%d] level %d, value %d, node %d, speed %d", p.x, p.y, result.level, result.value, result.node, result.speed);
	do_mymove(p.x, p.y);
}

void brain_end()
{
}

#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x, int y)
{
	HDC dc;
	HWND wnd;
	RECT rc;
	char c;
	wnd = GetForegroundWindow();
	dc = GetDC(wnd);
	GetClientRect(wnd, &rc);
	c = (char)(board[x][y] + '0');
	TextOut(dc, rc.right - 15, 3, &c, 1);
	ReleaseDC(wnd, dc);
}

#endif