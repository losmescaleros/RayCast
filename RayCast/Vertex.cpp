#include "stdafx.h"

#include "Vertex.h"

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

Vertex Vertex::Difference(Vertex firstVertex, Vertex secondVertex)
{
	Vertex difference = Vertex(firstVertex.x - secondVertex.x,
		firstVertex.y - secondVertex.y,
		firstVertex.z - secondVertex.z);

	return difference;
}

Vertex Vertex::Sum(Vertex firstVertex, Vertex secondVertex)
{
	Vertex sum = Vertex(firstVertex.x + secondVertex.x,
		firstVertex.y + secondVertex.y,
		firstVertex.z + secondVertex.z);

	return sum;
}

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

float Vertex::CalculateMagnitude()
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

float Vertex::DotProduct(Vertex firstVertex, Vertex secondVertex)
{
	float product = firstVertex.x * secondVertex.x + firstVertex.y * secondVertex.y + firstVertex.z * secondVertex.z;

	return product;
}

Vertex Vertex::Normalize()
{
	float magnitude = this->CalculateMagnitude();

	return Vertex(this->x / magnitude, this->y / magnitude, this->z / magnitude);
}