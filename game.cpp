#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include "levelProcessor.h"
#include "analyzer.h"
#include "console.h"
#include <sys/timeb.h>


extern int boardSize;

extern int searchLevel;

static int nodeCount;

long long getSystemTime() {
	struct timeb t;
	ftime(&t);
	return 1000 * t.time + t.millitm;
}

point search(Color aiColor, Color** map)
{
	long long t = getSystemTime();
	//��ʼ��
	initGameMap(map);
	initScore(aiColor);

	points neighbors = getNeighbor();
	analyzeData data = getAnalyzeData(aiColor, neighbors);
	points ps = getExpandPoints(data, neighbors);

	int extreme = MIN_VALUE;
	point result = point();
	Color color = aiColor;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		setPoint(p, color, NULL, aiColor);
		int value = dfs(searchLevel - 1, getOtherColor(color), MIN_VALUE, extreme, aiColor);
		printf("(%d, %d) value: %d count: %d time: %lld ms \n", p.x, p.y, value, nodeCount, getSystemTime() - t);
		if (value >= extreme) {
			result = p;
			extreme = value;
		}
		setPoint(p, NULL, color, aiColor);
	}
	return result;
}

int dfs(int level, Color color, int parentMax, int parentMin, Color aiColor) {
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
		if (color == aiColor) {
			return MAX_VALUE;
		}
		if (color == getOtherColor(aiColor)) {
			return MIN_VALUE;
		}
	}
	//������չ�ڵ�
	points ps = getExpandPoints(data, neighbors);
	//������չ�ڵ�
	int extreme = color == aiColor ? MIN_VALUE : MAX_VALUE;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		setPoint(p, color, NULL, aiColor);
		if (color == aiColor) {
			int value = dfs(level - 1, getOtherColor(color), MIN_VALUE, extreme, aiColor);
			if (value > parentMin) {
				setPoint(p, NULL, color, aiColor);
				return value;
			}
			if (value > extreme) {
				extreme = value;
				//�����Ӯ�ˣ���ֱ�Ӽ������������
				if (extreme == MAX_VALUE) {
					setPoint(p, NULL, color, aiColor);
					return extreme;
				}
			}
		}
		if (color != aiColor) {
			int value = dfs(level - 1, getOtherColor(color), extreme, MAX_VALUE, aiColor);
			if (value < parentMax) {
				setPoint(p, NULL, color, aiColor);
				return value;
			}
			if (value < extreme) {
				extreme = value;
				//����Ѿ����ˣ���ֱ�Ӽ������������
				if (extreme == MIN_VALUE) {
					setPoint(p, NULL, color, aiColor);
					return extreme;
				}
			}
		}
		setPoint(p, NULL, color, aiColor);
	}
	return extreme;
}

void setPoint(point p, Color color, Color forwardColor, Color aiColor) {
	updateScore(p, color, forwardColor, aiColor);
	setColor(p.x, p.y, color);
}