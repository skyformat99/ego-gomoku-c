#pragma once
#include "unordered_set"
class pointHash
{
	unordered_set<int> set;
public:
	int count = 0;
	void add(int x, int y);
	void add(point p);
	void addMany(points ps);
	bool contains(int x, int y);
	bool contains(point p);
	pointHash();
	~pointHash();
	void reset();
};

