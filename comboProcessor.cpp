#include "stdafx.h"
#include "comboProcessor.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include  "gameMap.h"

static int currentLevel;

static long long startTime;

static long long limitTime;

comboResult result;

void setColor(point point, Color color, Color forwardColor, Color aiColor) {
	updateScore(point, color, forwardColor, aiColor);
	setColor(point.x, point.y, color);
}

points getComboAttackPoints(analyzeData data, ComboType comboType) {
	//如果有对方冲4，则防冲4
	points result;
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	//如果有对方活3，冲四
	if (data.threeDenfence.count > 0) {
		result.addMany(data.fourAttack);
		return result;
	}

	result.addMany(data.fourAttack);
	if (comboType == THREE_COMBO) {
		result.addMany(data.threeAttack);
	}
	return result;
}

points getComboDefencePoints(analyzeData data, ComboType comboType) {
	points result;
	//如果有对方冲4，则防冲4
	if (data.fourDefence.count > 0) {
		result.addMany(data.fourDefence);
		return result;
	}
	if (comboType == THREE_COMBO) {
		//如果有对方活3，则防活3或者冲四
		if (data.threeDenfence.count > 0) {
			result.addMany(data.threeDenfence);
			result.addMany(data.fourAttack);
			return result;
		}
	}
	return result;
}

bool dfsKill(Color color, Color targetColor, int level, ComboType comboType, point* father, point* grandFather) {
	//超时判断
	if (getSystemTime() - startTime > limitTime) {
		result.timeOut = true;
		return false;
	}

	if (level == 0) {
		result.node++;
		return false;
	}
	//分析对面有没有被迫形成潜在的三四（潜在四四的情形暂无）
	if (comboType == THREE_COMBO)
		if (targetColor == color) {
			if (father != nullptr) {
				points fatherPoints = getPointLinesNeighbor(*father);
				analyzeData data = getAnalyzeData(getOtherColor(color), fatherPoints);
				pointHash hash;
				for (int i = 0; i < data.fourAttack.count; i++) {
					hash.add(data.fourAttack.list[i]);
				}
				for (int i = 0; i < data.threeAttack.count; i++) {
					if (hash.contains(data.threeAttack.list[i])) {
						comboType = FOUR_COMBO;
						break;
					}
				}
			}
		}
	//分析前两步周围的点
	points basePoints;
	if (father == nullptr)
		basePoints = getNeighbor();
	else {
		if (grandFather == nullptr)
			basePoints = getPointLinesNeighbor(*father);
		else {
			points fatherPoints = getPointLinesNeighbor(*father);
			points grandPoints = getPointLinesNeighbor(*grandFather);
			pointHash hash;
			for (int i = 0; i < fatherPoints.count; i++) {
				point p = fatherPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(fatherPoints.list[i]);
				}
			}
			for (size_t i = 0; i < grandPoints.count; i++) {
				point p = grandPoints.list[i];
				if (!hash.contains(p)) {
					basePoints.add(p);
					hash.add(grandPoints.list[i]);
				}
			}
		}
	}
	analyzeData data = getAnalyzeData(color, basePoints);
	//如果对面形成活三，则转换会冲四
	if (comboType == THREE_COMBO) {
		if (color == targetColor && data.threeDenfence.count > 0) {
			comboType = FOUR_COMBO;
		}
	}
	if (color == targetColor) {
		if (data.fiveAttack.count > 0) {
			if (level == currentLevel) {
				result.p = data.fiveAttack.list[0];
				result.win = true;
			}
			return true;
		}
		points ps = getComboAttackPoints(data, comboType);
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p, NULL, color, targetColor);
			if (level == currentLevel && value) {
				result.p = p;
				result.win = true;
			}
			if (value)
				return true;
		}
		return false;
	}
	else {
		if (data.fiveAttack.count > 0) {
			return false;
		}
		points ps = getComboDefencePoints(data, comboType);
		//如果没有能防的则结束
		if (ps.count == 0) {
			return false;
		}
		for (int i = 0; i < ps.count; i++)
		{
			point p = ps.list[i];
			setColor(p, color, NULL, targetColor);
			bool value = dfsKill(getOtherColor(color), targetColor, level - 1, comboType, &p, father);
			setColor(p, NULL, color, targetColor);
			if (!value) {
				return false;
			}
		}
		return true;
	}
}

comboResult canKill(Color targetColor, int level, long long startTimeValue, long long limitTimeValue)
{
	if (level % 2 == 0) {
		level++;
	}
	startTime = startTimeValue;
	limitTime = limitTimeValue;
	currentLevel = level;
	//计算我方四连杀
	result.reset();
	dfsKill(targetColor, targetColor,
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = true;
		return result;
	}
	//计算对手四连杀
	result.reset();
	dfsKill(getOtherColor(targetColor), getOtherColor(targetColor),
		level, FOUR_COMBO, nullptr, nullptr);
	if (result.win) {
		result.win = false;
		return result;
	}

	//计算我方三连杀
	result.reset();
	dfsKill(targetColor, targetColor,
		level, THREE_COMBO, nullptr, nullptr);
	if (result.win) {
		result.fourWin = false;
		return result;
	}
	return result;
}
