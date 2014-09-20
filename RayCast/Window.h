#ifndef WINDOW_F

#define WINDOW_F

#include "Vertex.h"
#include "Ray.h"

class Window
{



public:
	int pixelWidth;
	int pixelHeight;
	float width;
	float height;
	float backClippingDistance;

	Vertex topLeft;
	Vertex eye;
	Vertex lookUp;
	Vertex lookAt;

	Window();
	Window(int pixelWidth, int pixelHeight, float width, float height, Vertex topLeft, Vertex lookUp, Vertex lookAt, Vertex eye, float backClippingDistance);

	Vertex GetGlobalCoordinates(int x, int y);
};

#endif