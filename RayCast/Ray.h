#ifndef RAY_F

#define RAY_F

class Ray
{

	void Normalize();

public:
	float x;
	float y;
	float z;

	Ray();
	Ray(float x, float y, float z);

	static float CalculateMagnitude(float x, float y, float z);
};

#endif