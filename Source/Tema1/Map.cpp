#include "Map.h"

Map::Map()
{
	width = 10;
	height = 10;
	x = -5;
	y = -5;
}

Map::Map(int w, int h, int xu, int yu)
{
	width = w;
	height = h;
	x = xu;
	y = yu;
}