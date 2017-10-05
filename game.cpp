#include "stdafx.h"
#include "game.h"
#include "score.h"
#include "gameMap.h"
#include "winChecker.h"
#include <sys/timeb.h>


extern int boardSize;

extern int searchLevel;

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

	points ps = getNeighbor();

	int max = MIN_VALUE;
	point result = point();
	Color color = aiColor;
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		setPoint(p, color, NULL, aiColor);
		int value = dfs(searchLevel, color, MIN_VALUE, MAX_VALUE, aiColor);
		printf("(%d, %d) value: %d time: %ld ms \n", p.x, p.y, value, getSystemTime() - t);
		if (value >= max) {
			result = p;
			max = value;
		}
		setPoint(p, NULL, color, aiColor);
	}
	return result;
}

int dfs(int level, Color color, int parentMax, int parentMin, Color aiColor) {
	//todo �Ƿ�ʱ�ж�

	//Ҷ�ӷ�������
	if (level == 0) {
		return getScoreValue();
	}
	//��Ӯ�ж�
	Color winColor = win(getMap());
	if (winColor == aiColor) {
		return MAX_VALUE;
	}
	if (winColor == getOtherColor(aiColor)) {
		return MIN_VALUE;
	}
	//������չ�ڵ�
	points ps = getNeighbor();
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