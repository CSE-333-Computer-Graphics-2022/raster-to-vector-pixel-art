#include "Vector2D.h"

Vector2D::Vector2D(float xval, float yval)
{
	x = xval;
	y = yval;
}

void Vector2D::normalize()
{
	if (x * x + y * y > 0)
	{
		float mag = sqrt(x * x + y * y);
		x = x / mag;
		y = y / mag;
	}
}

float Vector2D::magnitude()
{
	return sqrt(x * x + y * y);
}

Vector2D operator+(Vector2D a, Vector2D b)
{
	return Vector2D(a.x + b.x, a.y + b.y);
}

Vector2D operator-(Vector2D a, Vector2D b)
{
	return Vector2D(a.x - b.x, a.y - b.y);
}

float operator*(Vector2D a, Vector2D b)
{
	return a.x*b.x + a.y*b.y;
}
