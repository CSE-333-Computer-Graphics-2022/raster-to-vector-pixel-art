#pragma once

#include <math.h>

class Vector2D
{
public:
	float x;
	float y;

	Vector2D(float xval, float yval);

	void normalize();
	float magnitude();

	friend Vector2D operator + (Vector2D a, Vector2D b);
	friend Vector2D operator - (Vector2D a, Vector2D b);
	friend float operator * (Vector2D a, Vector2D b);
};