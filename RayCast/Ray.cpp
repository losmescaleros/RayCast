#include "stdafx.h"

#include "Ray.h"

Ray::Ray()
{
	Ray::Ray(0, 0, 0);
}

Ray::Ray(float x, float y, float z)
{
	Ray::x = x;
	Ray::y = y;
	Ray::z = z;

	this->Normalize();
}

void Ray::Normalize()
{
	float magnitude = this->CalculateMagnitude(x, y, z);

	x = x / magnitude;
	y = y / magnitude;
	z = z / magnitude;
}

float Ray::CalculateMagnitude(float x, float y, float z)
{
	return sqrt(x * x + y * y + z * z);
}