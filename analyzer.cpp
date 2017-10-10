#include "stdafx.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "score.h"
#include "unordered_map"


static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };
analyzeData getAnalyzeData(Color color, points ps) {
	return getAnalyzeData(color, ps, false);
}

analyzeData getAnalyzeData(Color color, points ps, bool moreAnalyze)
{
	Color** map = getMap();
	analyzeData result;
	pointHash fiveAttackHash;
	pointHash fourAttackHash;
	pointHash fourDefenceHash;
	pointHash threeAttackHash;
	pointHash threeDenfenceHash;
	pointHash twoAttackHash;

	//��һά��ʾ��ǰ�����ϣֵ���ڶ�λ��ʾ����Ķ����Ƽ�����, ��λ����ʶ��ͬ����ĸ���
	//�ݲ�������ͬ����ĸ���
	unordered_map<int, int> weakFourDefenceMap;
	unordered_map<int, int> weakThreeDefenceMap;
	pointHash doubleWeakFourDefenceHash;
	pointHash weakThreeAndFourDefenceHash;
	pointHash doubleWeakThreeDefenceHash;

	for (int i = 0; i < ps.count; i++)
	{
		point p = ps.list[i];
		for (int direct = 0; direct < 4; direct++) {
			int x = p.x;
			int y = p.y;
			Color otherColor = getOtherColor(color);
			for (int k = 0; k < 5; k++) {
				int colorCount = getColorCount(color, x, y, direct);
				int otherColorCount = getColorCount(otherColor, x, y, direct);
				if (otherColorCount == 0 && colorCount == 4) {
					if (!fiveAttackHash.contains(p)) {
						result.fiveAttack.add(p);
						fiveAttackHash.add(p);
					}
				}
				if (otherColorCount == 0 && colorCount == 3) {
					if (!fourAttackHash.contains(p)) {
						result.fourAttack.add(p);
						fourAttackHash.add(p);
					}
				}
				if (otherColorCount == 4 && colorCount == 0) {
					if (!fourDefenceHash.contains(p)) {
						result.fourDefence.add(p);
						fourDefenceHash.add(p);
					}
				}
				if (otherColorCount == 0 && colorCount == 1) {
					if (!twoAttackHash.contains(p)) {
						result.twoAttack.add(p);
						twoAttackHash.add(p);
					}
				}
				if (otherColorCount == 0 && colorCount == 2) {
					if (k != 0 && k != 4) {
						int headX = x - directX[direct] * 4;
						int headY = y - directY[direct] * 4;
						if (reachable(headX, headY)) {
							Color headColor = map[headX][headY];
							Color tailColor = map[x][y];
							if (headColor == NULL && tailColor == NULL) {
								int sideX = x + directX[direct];
								int sideY = y + directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
								sideX = headX - directX[direct];
								sideY = headY - directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
							}
							if (headColor == NULL && tailColor != NULL) {
								int sideX = x + directX[direct];
								int sideY = y + directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
							}
							if (headColor != NULL && tailColor == NULL) {
								int sideX = headX - directX[direct];
								int sideY = headY - directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeAttackHash.contains(p)) {
											result.threeAttack.add(p);
											threeAttackHash.add(p);
										}
									}
								}
							}
						}
					}
				}
				if (otherColorCount == 3 && colorCount == 0) {
					int headX = x - directX[direct] * 4;
					int headY = y - directY[direct] * 4;
					if (reachable(headX, headY)) {
						Color headColor = map[headX][headY];
						Color tailColor = map[x][y];
						if (headColor != NULL && tailColor == NULL) {
							if (map[x - directX[direct]][y - directY[direct]] != NULL) {
								int sideX = headX - directX[direct];
								int sideY = headY - directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeDenfenceHash.contains(p)) {
											result.threeDenfence.add(p);
											threeDenfenceHash.add(p);
										}
									}
								}
							}
						}
						if (tailColor != NULL && headColor == NULL) {
							if (map[headX + directX[direct]][headY + directY[direct]] != NULL) {
								int sideX = x + directX[direct];
								int sideY = y + directY[direct];
								if (reachable(sideX, sideY)) {
									Color sideColor = map[sideX][sideY];
									if (sideColor == NULL) {
										if (!threeDenfenceHash.contains(p)) {
											result.threeDenfence.add(p);
											threeDenfenceHash.add(p);
										}
									}
								}
							}
						}
						if (headColor == NULL && tailColor == NULL) {
							int sideX = x + directX[direct];
							int sideY = y + directY[direct];
							if (reachable(sideX, sideY) &&
								map[sideX][sideY] == NULL) {
								if (!threeDenfenceHash.contains(p)) {
									result.threeDenfence.add(p);
									threeDenfenceHash.add(p);
								}
							}
							sideX = headX - directX[direct];
							sideY = headY - directY[direct];
							if (reachable(sideX, sideY) &&
								map[sideX][sideY] == NULL) {
								if (!threeDenfenceHash.contains(p)) {
									result.threeDenfence.add(p);
									threeDenfenceHash.add(p);
								}
							}
						}
					}
				}

				//���ӵķ���
				if (moreAnalyze) {
					if (otherColorCount == 3 && colorCount == 0) {
						//˫��
						//�˴�λ����������ж��Ǻ����������������ж�
						if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
							if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
								if (!doubleWeakFourDefenceHash.contains(p)) {
									doubleWeakFourDefenceHash.add(p);
									result.doubleWeakFourDefence.add(p);
								}
							}
						}
						//����
						if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
							if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
								if (!weakThreeAndFourDefenceHash.contains(p)) {
									weakThreeAndFourDefenceHash.add(p);
									result.weakThreeAndFourDefence.add(p);
								}
							}
						}

						if (weakFourDefenceMap.find(p.hash()) == weakFourDefenceMap.end()) {
							result.weakFourDefence.add(p);
						}
						weakFourDefenceMap[p.hash()] |= 1 << direct;
					}
					if (otherColorCount == 2 && colorCount == 0) {
						int headX = x - directX[direct] * 4;
						int headY = y - directY[direct] * 4;
						if (k != 0 && k != 4) {
							if (reachable(headX, headY)) {
								Color headColor = map[headX][headY];
								Color tailColor = map[x][y];
								if (headColor == NULL && tailColor == NULL) {
									int sideX = x + directX[direct];
									int sideY = y + directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//˫��
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//����
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
									sideX = headX - directX[direct];
									sideY = headY - directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//˫��
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//����
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
								}
								if (headColor == NULL && tailColor != NULL) {
									int sideX = x + directX[direct];
									int sideY = y + directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//˫��
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//����
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
								}
								if (headColor != NULL && tailColor == NULL) {
									int sideX = headX - directX[direct];
									int sideY = headY - directY[direct];
									if (reachable(sideX, sideY)) {
										Color sideColor = map[sideX][sideY];
										if (sideColor == NULL) {
											//˫��
											if (weakThreeDefenceMap.find(p.hash()) != weakThreeDefenceMap.end()) {
												if (weakThreeDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!doubleWeakThreeDefenceHash.contains(p)) {
														doubleWeakThreeDefenceHash.add(p);
														result.doubleWeakThreeDefence.add(p);
													}
												}
											}
											//����
											if (weakFourDefenceMap.find(p.hash()) != weakFourDefenceMap.end()) {
												if (weakFourDefenceMap[p.hash()] & (((1 << 4) - 1) - (1 << direct))) {
													if (!weakThreeAndFourDefenceHash.contains(p)) {
														weakThreeAndFourDefenceHash.add(p);
														result.weakThreeAndFourDefence.add(p);
													}
												}
											}

											if (weakThreeDefenceMap.find(p.hash()) == weakThreeDefenceMap.end()) {
												result.weakThreeDefence.add(p);
											}
											weakThreeDefenceMap[p.hash()] |= 1 << direct;
										}
									}
								}
							}
						}
					}
				}
				x += directX[direct];
				y += directY[direct];
				if (!reachable(x, y)) {
					break;
				}
			}
		}
	}
	return result;
}
