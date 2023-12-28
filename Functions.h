#pragma once

#include "Includes.h"

double distance(Vector2f p1, Vector2f p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}