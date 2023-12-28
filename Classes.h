#pragma once

#include "Includes.h"
#include "Enums.h"

class Collision
{
public:

	Vector2f position;
	CollisionOption collision_option;

	Collision() = default;

	Collision(Vector2f position, CollisionOption collision_option) : position(position), collision_option(collision_option) {}
};

class ViewingAngle
{
public:

	double min;
	double max;
	double step;
	double range;
	double rotate_angle;

	ViewingAngle() = default;

	ViewingAngle(double min, double max, double step, double range, double rotate_angle) : min(min), max(max), step(step), range(range), rotate_angle(rotate_angle) {}

	double average()
	{
		return (max + min) / 2;
	}
};