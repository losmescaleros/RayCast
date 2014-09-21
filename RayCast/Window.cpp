#include "stdafx.h"

#include "Window.h"

Window::Window()
{

}

/* Create a window object specifying pixel width and height and other options */
Window::Window(int pixelWidth, int pixelHeight, float width, float height, Vertex topLeft, Vertex lookUp, Vertex lookAt, Vertex eye, float backClippingDistance)
{
	this->pixelWidth = pixelWidth;
	this->pixelHeight = pixelHeight;
	this->width = width;
	this->height = height;
	this->topLeft = topLeft;
	this->eye = eye;
	this->backClippingDistance = backClippingDistance;
}

/* Find the global coordinates of a 2D location in the window */
Vertex Window::GetGlobalCoordinates(int x, int y)
{
	float globalX = this->topLeft.x + ((float)x / this->pixelWidth) * width;
	float globalY = this->topLeft.y - ((float)y / this->pixelHeight) * height;
	float globalZ = topLeft.z;

	return Vertex(globalX, globalY, globalZ);
}