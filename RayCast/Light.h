#ifndef LIGHT_F

#define LIGHT_F

#include "Vertex.h"
#include <windows.h>

#define DEFAULT_CONSTANT_ATTENUATION 1.0
#define DEFAULT_LINEAR_ATTENUATION 0.0
#define DEFAULT_QUADRATIC_ATTENUATION 0.0

class Light
{


public:
	Vertex location;
	float ambient;
	float diffuse;
	float specular;
	COLORREF color;

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	Light(Vertex location, float ambient, float diffuse, float specular, COLORREF color);
	Light(Vertex location, float ambient, float diffuse, float specular, COLORREF color, float constantAttenuation, float linearAttenuation, float quadraticAttenuation);
};

#endif