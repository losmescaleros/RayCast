#include "stdafx.h"

#include "Vertex.h"

/* Create a vertex using an x, y, and z coordinate */
Vertex::Vertex(float x, float y, float z)
{
	Vertex::x = x;
	Vertex::y = y;
	Vertex::z = z;
}

Vertex::Vertex()
{
	Vertex::x = 0.0f;
	Vertex::y = 0.0f;
	Vertex::z = 0.0f;
}

/* The subtraction of two vertices. Subtract the first vertex from the second. */
Vertex Vertex::Difference(Vertex firstVertex, Vertex secondVertex)
{
	Vertex difference = Vertex(firstVertex.x - secondVertex.x,
		firstVertex.y - secondVertex.y,
		firstVertex.z - secondVertex.z);

	return difference;
}

/* Add two vertices */
Vertex Vertex::Sum(Vertex firstVertex, Vertex secondVertex)
{
	Vertex sum = Vertex(firstVertex.x + secondVertex.x,
		firstVertex.y + secondVertex.y,
		firstVertex.z + secondVertex.z);

	return sum;
}

/* Find the cross-product of two vertices */
Vertex Vertex::CrossProduct(Vertex firstVertex, Vertex secondVertex)
{
	// Nx = AyBz - AzBy

	float x = firstVertex.y * secondVertex.z - firstVertex.z * secondVertex.y;
	// Ny = AzBx - AxBz

	float y = firstVertex.z * secondVertex.x - firstVertex.x * secondVertex.z;
	// Nz = AxBy - AyBx

	float z = firstVertex.x * secondVertex.y - firstVertex.y * secondVertex.x;

	Vertex crossProduct = Vertex(x, y, z);

	return crossProduct;
}

/* Calculate the magnitude (distance) of a vector */
float Vertex::CalculateMagnitude()
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

/* Find the dot product of two vectors */
float Vertex::DotProduct(Vertex firstVertex, Vertex secondVertex)
{
	float product = firstVertex.x * secondVertex.x + firstVertex.y * secondVertex.y + firstVertex.z * secondVertex.z;

	return product;
}

/* Normalize a vector to be a unit vector */
Vertex Vertex::Normalize()
{
	float magnitude = this->CalculateMagnitude();

	return Vertex(this->x / magnitude, this->y / magnitude, this->z / magnitude);
}