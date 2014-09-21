// RayCast.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RayCast.h"

#define MAX_LOADSTRING 100
#define DEFAULT_PIXEL_WIDTH 256
#define DEFAULT_PIXEL_HEIGHT 256
#define DEFAULT_WIDTH 2.0
#define DEFAULT_HEIGHT 2.0

static HWND sHwnd;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// My Global variables
// List of shapes
std::vector<tinyobj::shape_t> shapes;
std::vector<std::vector<float>> shapeNormals;

// Lights in the scene
std::vector<Light> lights;
// My window object
Window window;
COLORREF whiteColor = RGB(255, 255, 255);
// Default background color is black
COLORREF backgroundColor = RGB(0, 0, 0);

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void SetWindowHandle(void);
void SetPixel(int x, int y, COLORREF& color);
void DrawColor(void);
void SetWindowHandle(HWND hwnd);


// Forward declarations for my functions
void LoadWindowSettings(std::string windowFile, int* pixelWidth, int* pixelHeight, float* width, float* height);
void LoadLightSettings(std::string lightsFile);
bool IsIntWithinRange(int lower, int upper, int target);
void LoadObj(std::string inputFile);
bool TestShapes(std::vector<double> point, int* shapeIndex, int* triangleIndex);
COLORREF CalculateColor(Vertex point, tinyobj::shape_t shape, Triangle triangle);
Triangle GetTriangleFromShapesList(int shapeIndex, int triangleIndex);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	
	// Load a window file if there is one
	int pixWidth = 0;
	int pixHeight = 0;
	float width = 0;
	float height = 0;
	LoadWindowSettings("./window.txt", &pixWidth, &pixHeight, &width, &height);
	// Load any lights defined for the scene
	LoadLightSettings("./lights.txt");
	
	// This is the back clipping distance. The clipping plane is assumed to be parallel to the viewing plane
	float backClippingDistance = 5;
	Vertex topLeft = Vertex(-1, 1, -1);
	Vertex lookUp = Vertex(0, 1, 0);
	Vertex lookAt = Vertex(0, 0, 1);
	Vertex eye = Vertex(0, 0, -2);
	window = Window(pixWidth, pixHeight, width, height, topLeft, lookUp, lookAt, eye, backClippingDistance);

	

	// Load the OBJ file
	std::string inputFile = "./inputs/cube2.obj";
	LoadObj(inputFile);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RAYCAST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAYCAST));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAYCAST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_RAYCAST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, window.pixelWidth, window.pixelHeight, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	RECT rt;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		//hdc = BeginPaint(hWnd, &ps);
		//// TODO: Add any drawing code here...
		//EndPaint(hWnd, &ps);

				
		GetClientRect(hWnd, &rt);
		SetWindowHandle(hWnd);
		DrawColor();

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void LoadWindowSettings(std::string windowFile, int* pixelWidth, int* pixelHeight, float* width, float* height)
{
	std::ifstream input(windowFile);
	// Set width and heigh to defaults initially
	*pixelWidth = DEFAULT_PIXEL_WIDTH;
	*pixelHeight = DEFAULT_PIXEL_HEIGHT;
	*width = DEFAULT_WIDTH;
	*height = DEFAULT_HEIGHT;
	int wPix;
	int hPix;
	float w;
	float h;

	std::string line;
	while (std::getline(input, line))
	{
		std::istringstream iss(line);
		// If there was an error reading the file, use the default width and height
		if (!(iss >> wPix >> hPix >> w >> h))
		{
			
		}
		else
		{
			// Use the values read if they are valid
			if (h > 0 && w > 0)
			{
				*width = w;
				*height = h;
			}
			if (wPix > 0 && hPix > 0)
			{
				*pixelWidth = wPix;
				*pixelHeight = hPix;
			}
		}
	}

	
}

/**
 * Read a file which defines any lights within the scene. It is assumed that the file consists of lines
 * each which define a single light. The line should provide x, y, and z coordinates as floats; red, green,
 * and blue intensity values as integers between 0 and 255; and ambient, diffuse, and specular terms as floats
 * between 0 and 1.0.
**/
void LoadLightSettings(std::string lightsFile)
{
	std::ifstream input(lightsFile);

	std::string line;
	int lineNumber = 1;
	while (std::getline(input, line))
	{
		float x;
		float y;
		float z;
		int red;
		int green;
		int blue;
		float ambient;
		float diffuse;
		float specular;

		std::istringstream iss(line);
		// If there was an error reading the file, use the default width and height
		if (!(iss >> x >> y >> z >> red >> green >> blue >> ambient >> diffuse >> specular))
		{
			std::cout << "Could not read light input on line " << lineNumber << std::endl;
		}
		else
		{
			if (IsIntWithinRange(0, 255, red) && IsIntWithinRange(0, 255, blue) && IsIntWithinRange(0, 255, green))
			{
				COLORREF color = RGB(red, green, blue);
				Light light = Light(Vertex(x, y, z), ambient, diffuse, specular, color);
				lights.push_back(light);
			}
			else
			{
				std::cout << "Invalid color range specified at line " << lineNumber << std::endl;
			}
		}
	}
}

/* Is a target integer within a given range */
bool IsIntWithinRange(int lower, int upper, int target)
{
	return target <= upper && target >= lower;
}

/* Read in a single .OBJ file*/
void LoadObj(std::string inputFile)
{	
	std::vector<tinyobj::material_t> materials;
	
	std::string errors = tinyobj::LoadObj(shapes, inputFile.c_str(), "./inputs/");

	
	if (!errors.empty()){		
		std::cerr << errors << std::endl;
		exit(1);
	}
	
	std::cout << "# of shapes: " << shapes.size() << std::endl;

}

/* Test each of the shapes loaded to see if there is an intersection. If so, set the shape index
  and triangle index values so that we can reference the triangle from the shapes list 
*/
bool TestShapes(Vertex point, int* shapeIndex, int* triangleIndex)
{
	bool hitsSomething = false;

	float t = window.backClippingDistance;

	for (int s = 0; s < shapes.size(); s++)
	{
		for (int i = 0; i < shapes[s].mesh.indices.size() / 3; i++)
		{
			int indexA = shapes[s].mesh.indices[3 * i];
			int indexB = shapes[s].mesh.indices[3 * i + 1];
			int indexC = shapes[s].mesh.indices[3 * i + 2];
			Vertex vertexA = Vertex(shapes[s].mesh.positions[3 * indexA], shapes[s].mesh.positions[3 * indexA + 1], shapes[s].mesh.positions[3 * indexA + 2]);
			Vertex vertexB = Vertex(shapes[s].mesh.positions[3 * indexB], shapes[s].mesh.positions[3 * indexB + 1], shapes[s].mesh.positions[3 * indexB + 2]);
			Vertex vertexC = Vertex(shapes[s].mesh.positions[3 * indexC], shapes[s].mesh.positions[3 * indexC + 1], shapes[s].mesh.positions[3 * indexC + 2]);

			Triangle triangle = Triangle(vertexA, vertexB, vertexC);

			//bool hitsThis = triangle.Hit(0, &t, point, window.eye);
			Vertex dir = Vertex::Difference(point, window.eye);
			float dirTest = Vertex::DotProduct(dir, window.lookAt);
			bool hitsThis = false;
			
			hitsThis = triangle.HitMT(point, window.eye, &t);
			
			
			//bool hitsThis = RayTriIntersect(eyePosition, point, vertexA, vertexB, vertexC, &t);

			if (hitsThis)
			{
				*shapeIndex = s;
				*triangleIndex = i;
				hitsSomething = true;
			}
		}
	}

	return hitsSomething;
}

/* Calculate the color intensity for the point given its material.
*/
COLORREF CalculateColor(Vertex point, tinyobj::shape_t shape, Triangle triangle)
{
	COLORREF color;
	float redIntensity = 0;
	float blueIntensity = 0;
	float greenIntensity = 0;

	for (int i = 0; i < lights.size(); i++)
	{
		Vertex lightVector = Vertex::Difference(lights[i].location, point);
		float distance = lightVector.CalculateMagnitude();
		lightVector = lightVector.Normalize();

		Vertex vectorA = Vertex::Difference(triangle.vertexB, triangle.vertexA);
		Vertex vectorB = Vertex::Difference(triangle.vertexC, triangle.vertexA);

		Vertex normalVector = Vertex::CrossProduct(vectorA, vectorB);
		normalVector = normalVector.Normalize();

		Vertex testVector = Vertex::Difference(lights[i].location, triangle.vertexA);
		float test = Vertex::DotProduct(testVector, normalVector);
		if (test < 1)
		{
			normalVector.x = normalVector.x * -1;
			normalVector.y = normalVector.y * -1;
			normalVector.z = normalVector.z * -1;
		}


		float NdotL = Vertex::DotProduct(normalVector, lightVector);

		Vertex viewVector = Vertex::Difference(window.eye, point);
		viewVector = viewVector.Normalize();

		Vertex LPlusV = Vertex::Sum(lightVector, viewVector);

		float LPLusVMagnitude = LPlusV.CalculateMagnitude();

		Vertex H = Vertex(LPlusV.x / LPLusVMagnitude, LPlusV.y / LPLusVMagnitude, LPlusV.z / LPLusVMagnitude);

		float HdotN = Vertex::DotProduct(H, normalVector);

		float N = atof(shape.material.unknown_parameter["N"].c_str());
		float n = 128 * N / 1000;
		n = max(1, n);

		float attenuation = 1 / (lights[i].constantAttenuation +
			(lights[i].linearAttenuation * distance) +
			(lights[i].quadraticAttenuation * distance * distance));

		redIntensity += attenuation * (lights[i].ambient * shape.material.ambient[0] +
			lights[i].diffuse * shape.material.diffuse[0] * NdotL +
			lights[i].specular * shape.material.specular[0] * pow(HdotN, n));

		greenIntensity += attenuation * (lights[i].ambient * shape.material.ambient[1] +
			lights[i].diffuse * shape.material.diffuse[1] * NdotL +
			lights[i].specular * shape.material.specular[1] * pow(HdotN, n));

		blueIntensity += attenuation * (lights[i].ambient * shape.material.ambient[2] +
			lights[i].diffuse * shape.material.diffuse[2] * NdotL +
			lights[i].specular * shape.material.specular[2] * pow(HdotN, n));		
	}

	// Clamp the color intensities to 1 if they are over 1.
	if (redIntensity > 1){
		redIntensity = 1;
	}
	if (greenIntensity > 1){
		greenIntensity = 1;
	}
	if (blueIntensity > 1){
		blueIntensity = 1;
	}
	
	color = RGB(redIntensity * 255, greenIntensity * 255, blueIntensity * 255);

	return color;
}

/* Given a shape index and triangle index, get the relevant vertices of the triangle and return a Triangle object */
Triangle GetTriangleFromShapesList(int shapeIndex, int triangleIndex)
{
	int indexA = shapes[shapeIndex].mesh.indices[3 * triangleIndex];
	int indexB = shapes[shapeIndex].mesh.indices[3 * triangleIndex + 1];
	int indexC = shapes[shapeIndex].mesh.indices[3 * triangleIndex + 2];

	Vertex vertexA = Vertex(shapes[shapeIndex].mesh.positions[3 * indexA], shapes[shapeIndex].mesh.positions[3 * indexA + 1], shapes[shapeIndex].mesh.positions[3 * indexA + 2]);
	Vertex vertexB = Vertex(shapes[shapeIndex].mesh.positions[3 * indexB], shapes[shapeIndex].mesh.positions[3 * indexB + 1], shapes[shapeIndex].mesh.positions[3 * indexB + 2]);
	Vertex vertexC = Vertex(shapes[shapeIndex].mesh.positions[3 * indexC], shapes[shapeIndex].mesh.positions[3 * indexC + 1], shapes[shapeIndex].mesh.positions[3 * indexC + 2]);

	Triangle triangle = Triangle(vertexA, vertexB, vertexC);

	return triangle;
}

/* The implementation of the raycasting algorithm. For each pixel, determine if there is a triangle hit by the view ray, 
	find the closest one, calculate its color, and set that pixel as that color.
*/
void DrawColor()
{
	for (int y = 1; y <= window.pixelHeight; y++){
		for (int x = 1; x <= window.pixelWidth; x++){
			if (x == 255 && y == 0)
			{
				bool test2 = true;
			}

			// Find the pixel in world coordinates
			Vertex globalCoordinates = window.GetGlobalCoordinates(x, y);
			int shapeIndex = -1;
			int triangleIndex = -1;
			// Does the ray hit something?
			bool doesHit = TestShapes(globalCoordinates, &shapeIndex, &triangleIndex);
			
			
			if (doesHit)
			{
				// Find the triangle we need
				Triangle triangle = GetTriangleFromShapesList(shapeIndex, triangleIndex);

				// Calculate the pixel's color
				tinyobj::shape_t shape = shapes[shapeIndex];

				COLORREF color = CalculateColor(globalCoordinates, shape, triangle);
				// Set the pixel's color
				SetPixel(x, y, color);
			}
			else
			{
				SetPixel(x, y, backgroundColor);
			}
		}
	}
}

/* Set a certain pixel to a certain color */
void SetPixel(int x, int y, COLORREF& color)
{
	if (sHwnd == NULL)
	{
		exit(0);
	}
	HDC hdc = GetDC(sHwnd);
	SetPixel(hdc, x, y, color);
	ReleaseDC(sHwnd, hdc);
	return;
}

void SetWindowHandle(HWND hwnd)
{
	sHwnd = hwnd;
}