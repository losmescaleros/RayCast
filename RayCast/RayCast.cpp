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

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// My Global variables
std::vector<tinyobj::shape_t> shapes;
std::vector<std::vector<float>> shapeNormals;

std::vector<int> eyePosition;
std::vector<int> lookAtVector;
std::vector<int> lookUpVector;
std::vector<int> windowTopLeft;
std::vector<int> windowBottomRight;
std::vector<int> windowCenterPosition;

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


	for (int i = 0; i < shapes.size(); i++)
	{
		// Get vector A
		std::vector<float> vectorA = { 0, 0, 0 };
		vectorA[0] = abs(shapes[i].mesh.positions[0] - shapes[i].mesh.positions[3]);
		vectorA[1] = abs(shapes[i].mesh.positions[1] - shapes[i].mesh.positions[4]);
		vectorA[2] = abs(shapes[i].mesh.positions[2] - shapes[i].mesh.positions[5]);
		// Get vector B
		std::vector<float> vectorB = { 0, 0, 0 };
		vectorB[0] = abs(shapes[i].mesh.positions[0] - shapes[i].mesh.positions[6]);
		vectorB[1] = abs(shapes[i].mesh.positions[1] - shapes[i].mesh.positions[7]);
		vectorB[2] = abs(shapes[i].mesh.positions[2] - shapes[i].mesh.positions[8]);

		std::vector<float> normalVector;
		// Nx = AyBz - AzBy
		normalVector.push_back(vectorA[1] * vectorB[2] - vectorA[2] * vectorB[1]);
		//shapeNormals[i][0] = vectorA[1] * vectorB[2] - vectorA[2] * vectorB[1];
		
		// Ny = AzBx - AxBz
		normalVector.push_back(vectorA[2] * vectorB[0] - vectorA[0] * vectorB[2]);
		//shapeNormals[i][1] = vectorA[2] * vectorB[0] - vectorA[0] * vectorB[2];

		// Nz = AxBy - AyBx
		normalVector.push_back(vectorA[0] * vectorB[1] - vectorA[1] * vectorB[0]);
		//shapeNormals[i][2] = vectorA[0] * vectorB[1] - vectorA[1] * vectorB[0];

		// Add the normal vector to the list, which should be one-to-one with the shapes list
		shapeNormals.push_back(normalVector);
	}

	std::cout << "# of shapes: " << shapes.size() << std::endl;


}

void DrawColor()
{
	for (int y = 0; y < W_HEIGHT_PIXELS; y++){
		for (int x = 0; x < W_WIDTH_PIXELS; x++){
			// Find the pixel in world coordinates
			float pixelX = windowTopLeft[0] + ((float)x / W_WIDTH_PIXELS) * W_WIDTH;
			float pixelY = windowTopLeft[1] - ((float)x / W_WIDTH_PIXELS) * W_WIDTH;
			float pixelZ = (float)windowCenterPosition[2];
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