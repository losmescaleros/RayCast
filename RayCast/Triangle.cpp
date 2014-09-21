#include "stdafx.h"
#define EPSILON 0.000001
#include "Triangle.h"
#include "Vertex.h"

/* Create a triangle with three vertices */
Triangle::Triangle(Vertex vertexA, Vertex vertexB, Vertex vertexC)
{
	this->vertexA = vertexA;
	this->vertexB = vertexB;
	this->vertexC = vertexC;
}

/* Implementation of the Moller Trumbore ray triangle intersection algorithm */
bool Triangle::HitMT(Vertex point, Vertex eye, float* tMax)
{
	Vertex d = Vertex::Difference(point, eye);
	d = d.Normalize();

	Vertex edge1 = Vertex::Difference(this->vertexB, this->vertexA);
	
	Vertex edge2 = Vertex::Difference(this->vertexC, this->vertexA);
	

	Vertex P = Vertex::CrossProduct(d, edge2);

	float det = Vertex::DotProduct(edge1, P);

	// Test for both sides of the triangle
	if (det <  EPSILON && det > -EPSILON)
	{
		return false;
	}

	float inverseDet = 1 / det;

	Vertex T = Vertex::Difference(eye, vertexA);
	

	float u = Vertex::DotProduct(T, P) * inverseDet;

	if (u < 0 || u > 1)
	{
		// Intersection lies outside of triangle
		return false;
	}

	Vertex Q = Vertex::CrossProduct(T, edge1);
	

	float v = Vertex::DotProduct(d, Q) * inverseDet;

	if (v < 0 || u + v > 1)
	{
		// Intersection is outside of triangle
		return false;
	}

	float t = Vertex::DotProduct(edge2, Q) * inverseDet;

	if (t > EPSILON && t < *tMax)
	{
		// This is the closest triangle seen so far
		*tMax = t;
		return true;
	}

	return false;
}

/* Implementation of ray triangle intersection algorithm using Cramer's rule and the example from
   chapter 4 of Fundamentals of Computer Graphics 3rd Edition
*/
bool Triangle::Hit(float tMin, float *tMax, Vertex point, Vertex eye)
{
	float a = vertexA.x - vertexB.x;
	float b = vertexA.y - vertexB.y;
	float c = vertexA.z - vertexB.z;
	float d = vertexA.x - vertexC.x;
	float e = vertexA.y - vertexC.y;
	float f = vertexA.z - vertexC.z;
	
	float g = point.x - eye.x;
	float h = point.y - eye.y;
	float i = point.z - eye.z;

	float mag = Ray::CalculateMagnitude(g, h, i);
	/*g = g / mag;
	h = h / mag;
	i = i / mag;*/

	float j = vertexA.x - eye.x;
	float k = vertexA.y - eye.y;
	float l = vertexA.z - eye.z;

	float M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);

	// Compute t
	float t = -(f*(a*k - j*b) + e*(j*c - a*l) + d*(b*l - k*c)) / M;

	// If t > previous t, then this shape is farther away and will be occluded
	if (t < tMin || t > *tMax)
	{
		return false;
	}

	float gamma = (i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c)) / M;
	if (gamma < 0 || gamma > 1)
	{
		return false;
	}

	float beta = (j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g)) / M;
	if (beta < 0 || beta >(1 - gamma))
	{
		return false;
	}

	*tMax = t;
	return true;
}