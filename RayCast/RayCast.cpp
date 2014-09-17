// RayCast.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RayCast.h"

#define MAX_LOADSTRING 100
#define W_HEIGHT 2
#define W_WIDTH 2
#define W_HEIGHT_PIXELS 256
#define W_WIDTH_PIXELS 256

static HWND sHwnd;

static COLORREF redColor = RGB(255, 0, 0);
static COLORREF blueColor = RGB(0, 0, 255);
static COLORREF greenColor = RGB(0, 255, 0);
static COLORREF blackColor = RGB(0, 0, 0);

double prevT;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// My Global variables
std::vector<tinyobj::shape_t> shapes;
std::vector<std::vector<float>> shapeNormals;

std::vector<double> eyePosition;
std::vector<double> lookAtVector;
std::vector<double> lookUpVector;
std::vector<double> windowTopLeft;
std::vector<double> windowBottomRight;
std::vector<double> windowCenterPosition;
std::vector<double> lightLocation;
std::vector<double> lightIntensity;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void SetWindowHandle(void);
void SetPixel(int x, int y, COLORREF& color);
void DrawColor(void);
void SetWindowHandle(HWND hwnd);
double CalculateVectorMagnitude(std::vector<double> vector);
std::vector<double> NormalizeVector(std::vector<double> vector);

// Forward declarations for my functions
void LoadObj(std::string inputFile);
std::vector<double> CalculateShapeNormalVector(tinyobj::shape_t shape);
bool RayTriIntersect(std::vector<double> eye,
	std::vector<double> point,
	std::vector<double> vertexA,
	std::vector<double> vertexB,
	std::vector<double> vertexC,
	double* previousT);
std::vector<double> ConvertLocalToGlobalCoordinates(std::vector<double> windowCoordinates, int pixelX, int pixelY);
bool TestShapes(std::vector<double> point, int* shapeIndex, int* triangleIndex);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Define the eye position
	eyePosition = { 0, 0, -2 };
	//windowCenterPosition = { 0, 0, -1 };
	lookAtVector = { 0, 0, 1 };
	lookUpVector = { 0, 1, 0 };
	windowTopLeft = { -1, 1, -1 };
	//windowBottomRight = { 1, -1, 1 };
	lightLocation = { 0, 5, 0 };
	lightIntensity = { 1, 1, 1 };

	std::vector<double> v = { 0, 2, 4 };

	double mag = CalculateVectorMagnitude(v);

	// Load the OBJ file
	//std::string inputFile = "cube.obj";
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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

std::vector<double> CalculateNormalVector(std::vector<double> vectorA, std::vector<double> vectorB)
{	
		std::vector<double> normalVector;
		// Nx = AyBz - AzBy
		normalVector.push_back(vectorA[1] * vectorB[2] - vectorA[2] * vectorB[1]);

		// Ny = AzBx - AxBz
		normalVector.push_back(vectorA[2] * vectorB[0] - vectorA[0] * vectorB[2]);

		// Nz = AxBy - AyBx
		normalVector.push_back(vectorA[0] * vectorB[1] - vectorA[1] * vectorB[0]);

		normalVector = NormalizeVector(normalVector);

		return normalVector;
}

double CalculateVectorMagnitude(std::vector<double> vector)
{
	double magntiude = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);

	return magntiude;
}

std::vector<double> NormalizeVector(std::vector<double> vector)
{
	double magnitude = CalculateVectorMagnitude(vector);

	std::vector<double> normalizedVector;
	normalizedVector.push_back(vector[0] / magnitude);

	normalizedVector.push_back(vector[1] / magnitude);

	normalizedVector.push_back(vector[2] / magnitude);

	return normalizedVector;
}

bool RayTriIntersect(std::vector<double> eye, 
	std::vector<double> point, 
	std::vector<double> vertexA, 
	std::vector<double> vertexB, 
	std::vector<double> vertexC, 
	double* previousT)
{
	double a = vertexA[0] - vertexB[0];
	double b = vertexA[1] - vertexB[1];
	double c = vertexA[2] - vertexB[2];
	double d = vertexA[0] - vertexC[0];
	double e = vertexA[1] - vertexC[1];
	double f = vertexA[2] - vertexC[2];
	double g = point[0] - eye[0];
	double h = point[1] - eye[1];
	double i = point[2] - eye[2];
	std::vector<double> dir = { g, h, i };
	double mag = CalculateVectorMagnitude(dir);

	g = g / mag;
	h = h / mag;
	i = i / mag;

	double j = vertexA[0] - eye[0];
	double k = vertexA[1] - eye[1];
	double l = vertexA[2] - eye[2];

	double M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);

	// Compute t
	double t = -(f*(a*k - j*b) + e*(j*c - a*l) + d*(b*l - k*c)) / M;

	// If t > previous t, then this shape is farther away and will be occluded
	if (t < 0 || t > *previousT)
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

	*previousT = t;

	return true;
}


bool TestShapes(std::vector<double> point, int* shapeIndex, int* triangleIndex)
{
	bool hitsSomething = false;

	double t = 5;

	for (int s = 0; s < shapes.size(); s++)
	{
		for (int i = 0; i < shapes[s].mesh.indices.size() / 3; i++)
		{
			int indexA = shapes[s].mesh.indices[3 * i];
			int indexB = shapes[s].mesh.indices[3 * i + 1];
			int indexC = shapes[s].mesh.indices[3 * i + 2];

			std::vector<double> vertexA = { shapes[s].mesh.positions[3 * indexA], shapes[s].mesh.positions[3 * indexA + 1], shapes[s].mesh.positions[3 * indexA + 2] };
			std::vector<double> vertexB = { shapes[s].mesh.positions[3 * indexB], shapes[s].mesh.positions[3 * indexB + 1], shapes[s].mesh.positions[3 * indexB + 2] };
			std::vector<double> vertexC = { shapes[s].mesh.positions[3 * indexC], shapes[s].mesh.positions[3 * indexC + 1], shapes[s].mesh.positions[3 * indexC + 2] };

			bool hitsThis = RayTriIntersect(eyePosition, point, vertexA, vertexB, vertexC, &t);

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


std::vector<double> ConvertLocalToGlobalCoordinates(std::vector<double> windowCoordinates, int pixelX, int pixelY)
{
	std::vector<double> globalCoordinates;

	globalCoordinates.push_back(windowCoordinates[0] + ((float) pixelX / W_WIDTH_PIXELS) * W_WIDTH);

	globalCoordinates.push_back(windowCoordinates[1] - ((float)pixelY / W_WIDTH_PIXELS) * W_WIDTH);

	globalCoordinates.push_back(windowCoordinates[2]);

	return globalCoordinates;
}


void DrawColor()
{
	for (int y = 0; y <= W_HEIGHT_PIXELS; y++){
		for (int x = 0; x <= W_WIDTH_PIXELS; x++){
			if (x == 128 && y == 160){
				bool something = true;
			}
			// Find the pixel in world coordinates
			std::vector<double> globalCoordinates = ConvertLocalToGlobalCoordinates(windowTopLeft, x, y);
			int shapeIndex = -1;
			int triangleIndex = -1;

			bool doesHit = TestShapes(globalCoordinates, &shapeIndex, &triangleIndex);
			
			
			if (doesHit)
			{
				if (globalCoordinates[0] == 0 && globalCoordinates[1] == 1 && globalCoordinates[2] == -1)
				{
					bool want = true;
				}
				// Calculate the pixel's color
				double Lx = lightLocation[0] - globalCoordinates[0];
				double Ly = lightLocation[1] - globalCoordinates[1];
				double Lz = lightLocation[2] - globalCoordinates[2];

				std::vector<double> lightVector = { Lx, Ly, Lz };
				lightVector = NormalizeVector(lightVector);

				int indexA = shapes[shapeIndex].mesh.indices[3 * triangleIndex];
				int indexB = shapes[shapeIndex].mesh.indices[3 * triangleIndex + 1];
				int indexC = shapes[shapeIndex].mesh.indices[3 * triangleIndex + 2];
				
				std::vector<double> vertexA = { shapes[shapeIndex].mesh.positions[3* indexA], shapes[shapeIndex].mesh.positions[3 * indexA + 1], shapes[shapeIndex].mesh.positions[3 * indexA + 2] };
				std::vector<double> vertexB = { shapes[shapeIndex].mesh.positions[3 * indexB], shapes[shapeIndex].mesh.positions[3 * indexB + 1], shapes[shapeIndex].mesh.positions[3 * indexB + 2] };
				std::vector<double> vertexC = { shapes[shapeIndex].mesh.positions[3 * indexC], shapes[shapeIndex].mesh.positions[3 * indexC + 1], shapes[shapeIndex].mesh.positions[3 * indexC + 2] };

				std::vector<double> vectorA = { vertexB[0] - vertexA[0], vertexB[1] - vertexA[1], vertexB[2] - vertexA[2] };
				std::vector<double> vectorB = { vertexC[0] - vertexA[0], vertexC[1] - vertexA[1], vertexC[2] - vertexA[2] };

				std::vector<double> normalVector = CalculateNormalVector(vectorA, vectorB);
				normalVector[0] = normalVector[0] * -1;
				normalVector[1] = normalVector[1] * -1;
				normalVector[2] = normalVector[2] * -1;
				double NdotL = normalVector[0] * lightVector[0] + normalVector[1] * lightVector[1] + normalVector[2] * lightVector[2];
				
				double Vx = eyePosition[0] - globalCoordinates[0];
				double Vy = eyePosition[1] - globalCoordinates[1];
				double Vz = eyePosition[2] - globalCoordinates[2];
				std::vector<double> viewVector = { Vx, Vy, Vz };
				viewVector = NormalizeVector(viewVector);
				std::vector<double> LPlusV = { lightVector[0] + viewVector[0], lightVector[1] + viewVector[1], lightVector[2] + viewVector[2] };

				double LPlusVMagnitude = CalculateVectorMagnitude(LPlusV);

				std::vector<double> H = { LPlusV[0] / LPlusVMagnitude, LPlusV[1] / LPlusVMagnitude, LPlusV[2] / LPlusVMagnitude };
				H = NormalizeVector(H);

				double HdotN = H[0] * normalVector[0] + H[1] * normalVector[1] + H[2] * normalVector[2];
				double N = atof(shapes[shapeIndex].material.unknown_parameter["N"].c_str());
				double n = 128 * N / 1000;
				n = max(1, n);

				/*double redIntensity = lightIntensity[0] * shapes[shapeIndex].material.ambient[0] +
					lightIntensity[1] * shapes[shapeIndex].material.diffuse[0] * NdotL + 
					lightIntensity[2] * shapes[shapeIndex].material.specular[0] * pow(HdotN, max(1, n));

				double greenIntensity = lightIntensity[0] * shapes[shapeIndex].material.ambient[1] +
					lightIntensity[1] * shapes[shapeIndex].material.diffuse[1] * NdotL + 
					lightIntensity[2] * shapes[shapeIndex].material.specular[1] * pow(HdotN, max(1, n));

				double blueIntensity = lightIntensity[0] * shapes[shapeIndex].material.ambient[2] +
					lightIntensity[1] * shapes[shapeIndex].material.diffuse[2] * NdotL + 
					lightIntensity[2] * shapes[shapeIndex].material.specular[2] * pow(HdotN, max(1, n));*/

				double redIntensity = lightIntensity[0] * shapes[shapeIndex].material.ambient[0] + 
					lightIntensity[1] * shapes[shapeIndex].material.diffuse[0] * NdotL + 
					lightIntensity[2] * shapes[shapeIndex].material.specular[0] * pow(HdotN, max(1, n));

				double greenIntensity = lightIntensity[0] * shapes[shapeIndex].material.ambient[1] + 
					lightIntensity[1] * shapes[shapeIndex].material.diffuse[1] * NdotL + 
					lightIntensity[2] * shapes[shapeIndex].material.specular[1] * pow(HdotN, max(1, n));

				double blueIntensity = lightIntensity[0] * shapes[shapeIndex].material.ambient[2] + 
					lightIntensity[1] * shapes[shapeIndex].material.diffuse[2] * NdotL + 
					lightIntensity[2] * shapes[shapeIndex].material.specular[2] * pow(HdotN, max(1, n));

				if (redIntensity > 1){
					redIntensity = 1;
				}
				if (greenIntensity > 1){
					greenIntensity = 1;
				}
				if (blueIntensity > 1){
					blueIntensity = 1;
				}
				COLORREF color = RGB(redIntensity*255, greenIntensity*255, blueIntensity*255);

				SetPixel(x, y, color);
			}
			else
			{
				SetPixel(x, y, blackColor);
			}
		}
	}
	/*for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 200; x++)
		{
			if (y < 30) SetPixel(x, y, redColor);
			else if (y < 60) SetPixel(x, y, greenColor);
			else if (y < 90) SetPixel(x, y, blueColor);
			else SetPixel(x, y, redColor);
		}
	}*/
}


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