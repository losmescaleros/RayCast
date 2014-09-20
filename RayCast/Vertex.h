#ifndef VERTEX_F

#define VERTEX_F

class Vertex
{


public:
	float x;
	float y;
	float z;

	Vertex();
	Vertex(float x, float y, float z);

	static Vertex Difference(Vertex firstVertex, Vertex secondVertex);

	static Vertex Sum(Vertex firstVertex, Vertex secondVertex);

	static Vertex CrossProduct(Vertex firstVertex, Vertex secondVertex);

	static float DotProduct(Vertex firstVertex, Vertex secondVertex);

	Vertex Normalize();

	float CalculateMagnitude();
};

#endif