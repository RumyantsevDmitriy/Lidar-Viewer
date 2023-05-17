#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <math.h>
#include "Components.h"

#pragma comment(lib, "OpenGL32.lib")


#define MainWindowSizeX		1600
#define MainWindowSizeY		958
#define GLWindowSizeX		1600
#define GLWindowSizeY		900

#define GLWindowPosX		0
#define GLWindowPosY		0

#define CMDCameraPos1		10
#define CMDCameraPos2		11
#define CMDCameraPos3		12
#define CMDCameraPos4		13
#define CMDCameraPosFree	14
#define CMDCameraOrtho		15
#define CMDCameraPersp		16
#define CMDOpenFile			17
#define CMDSaveFile			18
#define CMDExit				19


HDC     hDC;					/* device context */
HGLRC   hGLRC;					/* render context (opengl context) */
HWND    hWnd, GLWnd;			/* window */


bool cameraIsFree = false;
bool fileIsOpen = false;
bool fileIsOpenNow = false;
int extension = -1;
std::string fileName = {};
std::vector <Vector3> scan;

HMENU	CameraPosMenu, CameraModeMenu, OpenFileMenu;

Renderer renderer = Renderer(Camera(Vector3(3, 4, 3), Vector3(0, 1, 0)));

std::vector<Vector3> points = Vector3::CirclePoints(4, 6, Vector3(0, 3.5f, 0), Quaternion::EulerAngles(0.07f, 0.5f, 0.059f));

LRESULT CALLBACK MainWndProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

/**
* Функция, которая заканчивает работу программы
*/
void ExitSoftware()
{
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hGLRC);
	ReleaseDC(hWnd, hDC);
	DestroyWindow(hWnd);
	PostQuitMessage(0);
}

/**
* Функция, инициализирующая окно
*/
ATOM MainWndRegisterClass(HINSTANCE hInstance, HBRUSH BGColor, HCURSOR Cursor, HICON Icon)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)MainWndProcedure;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = Icon;
	wcex.hCursor = Cursor;
	wcex.hbrBackground = BGColor;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"SoftwareMain";
	wcex.hIconSm = Icon;

	return RegisterClassExW(&wcex);
}

/**
* Функция, задающая параметры холста
*/
BOOL CreateRenderContext(HINSTANCE hInstance, LPCWSTR mainWndName)
{
	hWnd = CreateWindowW(L"SoftwareMain", mainWndName, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, MainWindowSizeX, MainWindowSizeY, nullptr, nullptr, hInstance, nullptr);
	GLWnd = CreateWindowA("static", NULL, WS_VISIBLE | WS_CHILD, GLWindowPosX, GLWindowPosY, GLWindowSizeX, GLWindowSizeY, hWnd, NULL, NULL, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };
	int                   pixelFormat;
	hDC = GetDC(GLWnd);

	pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 32;
	pixelFormat = ChoosePixelFormat(hDC, &pixelFormatDescriptor);

	SetPixelFormat(hDC, pixelFormat, &pixelFormatDescriptor);
	DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pixelFormatDescriptor);
	hGLRC = wglCreateContext(hDC);

	return (hGLRC != NULL);
}

/**
* Функция, отвечающая за создание меню
*/
void MainWndAddMenus(HWND hWndMain)
{
	HMENU RootMenu = CreateMenu();

	CameraPosMenu = CreateMenu();
	CameraModeMenu = CreateMenu();
	OpenFileMenu = CreateMenu();

	AppendMenu(CameraPosMenu, MF_STRING, CMDCameraPos1, "Position 1");
	AppendMenu(CameraPosMenu, MF_STRING, CMDCameraPos2, "Position 2");
	AppendMenu(CameraPosMenu, MF_STRING, CMDCameraPos3, "Position 3");
	AppendMenu(CameraPosMenu, MF_STRING, CMDCameraPos4, "Position 4");
	AppendMenu(CameraPosMenu, MF_STRING, CMDCameraPosFree, "Free camera");

	AppendMenu(CameraModeMenu, MF_STRING, CMDCameraOrtho, "Ortho");
	AppendMenu(CameraModeMenu, MF_STRING, CMDCameraPersp, "Perspective");

	AppendMenu(OpenFileMenu, MF_STRING, CMDOpenFile, "Open");
	AppendMenu(OpenFileMenu, MF_STRING, CMDSaveFile, "Save as");
	AppendMenu(OpenFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(OpenFileMenu, MF_STRING, CMDExit, "Exit");

	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)OpenFileMenu, "File");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)CameraPosMenu, "Position");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)CameraModeMenu, "View mode");

	SetMenu(hWndMain, RootMenu);
}

/**
* Функция, отвечающая за открытие диалогового окна "Открыть" и обработку введенного имени
*/
void OpenFile()
{
	OPENFILENAME ofn;
	char szFile[260];
	char szFileTitle[260];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "obj\0*.*\0lidar\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.lpstrFileTitle[0] = '\0';
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.nFileExtension = 0;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		fileName = ofn.lpstrFile;

		fileIsOpenNow = true;
		if (fileName.substr(ofn.nFileExtension) == "obj")
		{
			extension = 0;
			fileIsOpen = true;
		}
		else if (fileName.substr(ofn.nFileExtension) == "lidar")
		{
			extension = 1;
			fileIsOpen = true;
		}
		else
		{
			MessageBox(hWnd, "Can not open file", "Notification", 0);
			fileIsOpen = false;
		}

		PostMessageA(hWnd, 0, 0, 0);
	}
	else
	{
		MessageBox(hWnd, "The file was not selected", "Notification", 0);
	}

}

/**
* Функция, отвечающая за открытие диалогового окна "Сохранить как", обработку введенного имени и сохраниение файла
*/
void SaveFile()
{
	HANDLE hf;
	OPENFILENAME ofn;
	char szFile[260];
	char szFileTitle[260];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "obj\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.lpstrFileTitle[0] = '\0';
	ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = NULL;
	std::string fileNameSave = {};

	if (fileIsOpen)
	{
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			fileNameSave = ofn.lpstrFile;
			if (fileNameSave.substr(ofn.nFileExtension) == "obj")
			{
				std::string s;
				std::ofstream in(ofn.lpstrFile);
				in << "o Lidar_scan" << std::endl;
				for (int i = 0; i < scan.size(); i++)
				{
					s = "";
					s.push_back('v');
					s.push_back(' ');
					s.insert(s.size(), std::to_string(scan[i].x));
					s.push_back(' ');
					s.insert(s.size(), std::to_string(scan[i].y));
					s.push_back(' ');
					s.insert(s.size(), std::to_string(scan[i].z));
					in << s << std::endl;

				}
				in.close();
			}
			else
			{
				MessageBox(hWnd, "Can not save file - it has to be .obj", "Notification", 0);
			}

		}
		else
		{
			MessageBox(hWnd, "Can not save file", "Notification", 0);
		}
	}
	else
	{
		MessageBox(hWnd, "Can not save file - you have to open file", "Notification", 0);
	}
}
