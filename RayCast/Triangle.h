#ifndef TRIANGLE_F

#define TRIANGLE_F

#include "Vertex.h"
#include "Ray.h"

class Triangle
{


public:
	Vertex vertexA;
	Vertex vertexB;
	Vertex vertexC;

	Triangle(Vertex vertexA, Vertex vertexB, Vertex vertexC);

	bool Hit(float tMin, float *tMax, Vertex point, Vertex eye);

	bool HitMT(Vertex point, Vertex eye, float* tMax);
};

#endif