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

// Forward declarations for my functions
void LoadObj(std::string inputFile);
std::vector<double> CalculateShapeNormalVector(tinyobj::shape_t shape);
int RayTriIntersect(std::vector<double> eye, std::vector<double> point);
std::vector<double> ConvertLocalToGlobalCoordinates(std::vector<double> windowCoordinates, int pixelX, int pixelY);

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
	windowCenterPosition = { 0, 0, -1 };
	lookAtVector = { 0, 0, 1 };
	lookUpVector = { 0, 1, 0 };
	windowTopLeft = { -1, 1, -1 };
	windowBottomRight = { 1, -1, -1 };
	lightLocation = { 0, 2, 0 };
	lightIntensity = { 1, 1, 1 };

	// Load the OBJ file
	//std::string inputFile = "cube.obj";
	std::string inputFile = "triangle.obj";
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
	

	std::string errors = tinyobj::LoadObj(shapes, inputFile.c_str());

	if (!errors.empty()){		
		std::cerr << errors << std::endl;
		exit(1);
	}
	
	std::cout << "# of shapes: " << shapes.size() << std::endl;

}

std::vector<double> CalculateShapeNormalVector(tinyobj::shape_t shape)
{	
		// Get vector A
		std::vector<double> vectorA = { 0, 0, 0 };
		vectorA[0] = shape.mesh.positions[3] - shape.mesh.positions[0];
		vectorA[1] = shape.mesh.positions[4] - shape.mesh.positions[1];
		vectorA[2] = shape.mesh.positions[5] - shape.mesh.positions[2];
		// Get vector B
		std::vector<double> vectorB = { 0, 0, 0 };
		vectorB[0] = shape.mesh.positions[6] - shape.mesh.positions[0];
		vectorB[1] = shape.mesh.positions[7] - shape.mesh.positions[1];
		vectorB[2] = shape.mesh.positions[8] - shape.mesh.positions[2];

		std::vector<double> normalVector;
		// Nx = AyBz - AzBy
		normalVector.push_back(vectorA[1] * vectorB[2] - vectorA[2] * vectorB[1]);

		// Ny = AzBx - AxBz
		normalVector.push_back(vectorA[2] * vectorB[0] - vectorA[0] * vectorB[2]);

		// Nz = AxBy - AyBx
		normalVector.push_back(vectorA[0] * vectorB[1] - vectorA[1] * vectorB[0]);

		return normalVector;
}

int RayTriIntersect(std::vector<double> eye, std::vector<double> point)
{
	int indexOfClosestShape = -1;
	double previousT = 3;

	for (int index = 0; index < shapes.size(); index++)
	{
		tinyobj::shape_t shape = shapes[index];

		double a = shape.mesh.positions[0] - shape.mesh.positions[3];
		double b = shape.mesh.positions[1] - shape.mesh.positions[4];
		double c = shape.mesh.positions[2] - shape.mesh.positions[5];
		double d = shape.mesh.positions[0] - shape.mesh.positions[6];
		double e = shape.mesh.positions[1] - shape.mesh.positions[7];
		double f = shape.mesh.positions[2] - shape.mesh.positions[8];
		double g = point[0] - eye[0];
		double h = point[1] - eye[1];
		double i = point[2] - eye[2];
		double j = shape.mesh.positions[0] - eye[0];
		double k = shape.mesh.positions[1] - eye[1];
		double l = shape.mesh.positions[2] - eye[2];

		double M = a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h - e*g);

		// Compute t
		double t = -(f*(a*k - j*b) + e*(j*c - a*l) + d*(b*l - k*c)) / M;

		// If t > previous t, then this shape is farther away and will be occluded
		if (t < 0 || t > previousT)
		{
			continue;
		}

		double gamma = (i*(a*k - j*b) + h*(j*c - a*l) + g*(b*l - k*c)) / M;
		if (gamma < 0 || gamma > 1)
		{
			continue;
		}

		double beta = (j*(e*i - h*f) + k*(g*f - d*i) + l*(d*h - e*g)) / M;
		if (beta < 0 || beta >(1 - gamma))
		{
			continue;
		}

		previousT = t;
		indexOfClosestShape = index;
	}
	
	return indexOfClosestShape;
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
			if (x == 128 && y == 128){
				bool something = true;
			}
			// Find the pixel in world coordinates
			std::vector<double> globalCoordinates = ConvertLocalToGlobalCoordinates(windowTopLeft, x, y);

			// Make a hit record for this pixel location
			int lastIntersectedShape = RayTriIntersect(eyePosition, globalCoordinates);
			
			
			if (lastIntersectedShape != -1)
			{
				tinyobj::shape_t shape = shapes[lastIntersectedShape];
				if (globalCoordinates[0] == 0 && globalCoordinates[1] == 1 && globalCoordinates[2] == -1)
				{
					bool want = true;
				}
				// Calculate the pixel's color
				double Lx = globalCoordinates[0] - lightLocation[0];
				double Ly = globalCoordinates[1] - lightLocation[1];
				double Lz = globalCoordinates[2] - lightLocation[2];

				std::vector<double> normalVector = CalculateShapeNormalVector(shape);
				double NdotL = normalVector[0] * Lx + normalVector[1] * Ly + normalVector[2] * Lz;
				
				double Vx = globalCoordinates[0] - eyePosition[0];
				double Vy = globalCoordinates[1] - eyePosition[1];
				double Vz = globalCoordinates[2] - eyePosition[2];
				std::vector<double> LPlusV = { Lx + Vx, Ly + Vy, Lz + Vz };

				double LPlusVMagnitude = sqrt(pow(LPlusV[0], 2) + pow(LPlusV[1], 2) + pow(LPlusV[2], 2));

				std::vector<double> H = { LPlusV[0] / LPlusVMagnitude, LPlusV[1] / LPlusVMagnitude, LPlusV[2] / LPlusVMagnitude };

				double HdotN = H[0] * normalVector[0] + H[1] * normalVector[1] + H[2] * normalVector[2];

				//double redIntensity = /*max((*/lightIntensity[0] * shape.material.ambient[0] + lightIntensity[0] * shape.material.diffuse[0] * NdotL +
				//	lightIntensity[0] * shape.material.specular[0] * HdotN/*), 0)*/;

				//double greenIntensity = /*max((*/lightIntensity[1] * shape.material.ambient[1] + lightIntensity[1] * shape.material.diffuse[1] * NdotL +
				//	lightIntensity[1] * shape.material.specular[1] * HdotN/*), 0)*/;

				//double blueIntensity = /*max((*/lightIntensity[2] * shape.material.ambient[2] + lightIntensity[2] * shape.material.diffuse[2] * NdotL +
				//	lightIntensity[2] * shape.material.specular[2] * HdotN/*), 0)*/;
				double redIntensity = shape.material.ambient[0] + shape.material.diffuse[0];
				double greenIntensity = shape.material.ambient[1] + shape.material.diffuse[1];
				double blueIntensity = shape.material.ambient[2] + shape.material.diffuse[2];

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