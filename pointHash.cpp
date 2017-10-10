#include "stdafx.h"
#include "pointHash.h"

extern int boardSize;

int getKey(int x, int y) {
	return x * 100 + y;
}

void pointHash::add(int x, int y)
{
	set.insert(getKey(x, y));
	count++;
}

void pointHash::add(point p)
{
	set.insert(getKey(p.x, p.y));
	count++;
}

void pointHash::addMany(points ps)
{
	for (int i = 0; i < ps.count; i++)
		add(ps.list[i]);
}

bool pointHash::contains(int x, int y)
{
	return set.find(getKey(x, y)) != set.end();
}

bool pointHash::contains(point p)
{
	return set.find(getKey(p.x, p.y)) != set.end();
}

pointHash::pointHash()
{
	reset();
}

pointHash::~pointHash()
{

}

void pointHash::reset()
{
	set.clear();
	count = 0;
}
