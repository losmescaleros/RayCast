#include "stdafx.h"
#include "Light.h"

Light::Light(Vertex location, float ambient, float diffuse, float specular, COLORREF color)
	: Light(location, ambient, diffuse, specular, color, DEFAULT_CONSTANT_ATTENUATION, DEFAULT_LINEAR_ATTENUATION, DEFAULT_QUADRATIC_ATTENUATION)
{
	
}

Light::Light(Vertex location, float ambient, float diffuse, float specular, COLORREF color, float constantAttenuation, float linearAttenuation, float quadraticAttenuation)
{
	this->location = location;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->color = color;

	this->constantAttenuation = constantAttenuation;
	this->linearAttenuation = linearAttenuation;
	this->quadraticAttenuation = quadraticAttenuation;
}