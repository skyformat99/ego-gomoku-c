#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "analyzer.h"
#include "console.h"
#include <sys/timeb.h>
#include "unordered_map"

extern int boardSize;

extern int searchLevel;

extern int timeOut;

extern bool debugEnable;

static int nodeCount;

static long long searchStartTime;

static bool timeOutEnable = false;

int currentLevel;

points currentPointsResult;

unordered_map<long long, point> cache;

unordered_map<long long, point> cacheLast;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

gameResult search(Color aiColor, Color** map)
{
	gameResult gameResult;
	searchStartTime = getSystemTime();
	timeOutEnable = false;
	//��ʼ��
	initGameMap(map);
	initScore(aiColor);

	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(aiColor, neighbors);
	points ps = getExpandPoints(data, neighbors);

	if (ps.count == 0) {
		gameResult.result = point(boardSize / 2, boardSize / 2);
		return gameResult;
	}

	if (ps.count == 1) {
		gameResult.result = ps.list[0];
		return gameResult;
	}

	for (int level = 2; level <= searchLevel; level += 2)
	{
		long long t = getSystemTime();
		nodeCount = 0;

		cacheLast = cache;
		cache.clear();
		currentPointsResult.clear();

		currentLevel = level;
		Color color = aiColor;
		int alpha = MIN_VALUE;
		int beta = MAX_VALUE;

		int value = dfs(level, color, aiColor, alpha, beta);

		if (timeOutEnable) {
			if (debugEnable)
				printf("time out\n");
			break;
		}

		if (!timeOutEnable) {
			gameResult.result = currentPointsResult.list[rand()%currentPointsResult.count];
			int speed = 0;
			if ((getSystemTime() - t) > 0)
				speed = (int)(nodeCount / ((getSystemTime() - t) / 1000.00) / 1000);
			gameResult.speed = speed;
			gameResult.node = nodeCount;
			gameResult.value = value;
			gameResult.level = level;
			if (debugEnable)
				printf("level %d, speed %d k, time %d ms\n", level, speed, getSystemTime() - t);
		}
		if (getSystemTime() - searchStartTime > timeOut / 6) {
			timeOutEnable = true;
		}
	}

	return gameResult;
}

/* �㴰�ڲ��Է�
*/
int dfs(int level, Color color, Color aiColor, int alpha, int beta) {
	if (getSystemTime() - searchStartTime > timeOut) {
		timeOutEnable = true;
	}
	if (timeOutEnable) {
		return 0;
	}
	//Ҷ�ӷ�������
	if (level == 0) {
		nodeCount++;
		return getScoreValue();
	}
	//��������
	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(color, neighbors);
	//��Ӯ�ж�
	if (data.fiveAttack.count > 0) {
		return MAX_VALUE;
	}
	//������չ�ڵ�
	points ps = getExpandPoints(data, neighbors);

	//�������Žڵ�˳��
	if (cacheLast.find(getMapHashCode()) != cacheLast.end()) {
		point p = cacheLast[getMapHashCode()];
		for (int i = 0; i < ps.count; i++)
			if (ps.list[i].x == p.x && ps.list[i].y == p.y) {
				for (int j = i; j > i; j--) {
					ps.list[j] = ps.list[j - 1];
				}
				ps.list[0] = p;
				break;
			}
	}

	//������չ�ڵ�
	int extreme = MIN_VALUE;
	point extremePoint;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		setPoint(p, color, NULL, aiColor);
		int value;
		if (i == 0)
			value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha);
		else {
			value = -dfs(level - 1, getOtherColor(color), aiColor, -alpha - 1, -alpha);
			if (value > alpha && value < beta) {
				value = -dfs(level - 1, getOtherColor(color), aiColor, -beta, -alpha);
			}
		}
		setPoint(p, NULL, color, aiColor);

		if (level == currentLevel) {
			if (debugEnable)
				printf("(%d, %d) value: %d count: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - searchStartTime);
			if (value >= extreme) {
				if (value > extreme) {
					currentPointsResult.clear();
				}
				currentPointsResult.add(p);
			}
		}

		if (value >= extreme) {
			extreme = value;
			extremePoint = p;

			if (value > alpha) {
				alpha = value;
				if (value > beta) {
					return value;
				}
			}
		}
	}
	cache[getMapHashCode()] = extremePoint;
	return extreme;
}

void setPoint(point p, Color color, Color forwardColor, Color aiColor) {
	updateScore(p, color, forwardColor, aiColor);
	setColor(p.x, p.y, color);
}