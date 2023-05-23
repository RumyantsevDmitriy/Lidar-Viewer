#include <vector>
#include <Windows.h>
#include <gl/freeglut.h>

#include "LidarViewer.h"
#include <filesystem>

/**
* \brief Обработчик событий
* Функция, которая обрабатывает различные внешние команды: создание окна, его закрытие, нажатие на кнопки и т.д.
*/
LRESULT CALLBACK MainWndProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case CMDOpenFile:
			OpenFile();
			break;

		case CMDSaveFile:
			SaveFile();
			break;

		case CMDExit:
			PostQuitMessage(0);
			break;

		case CMDCameraOrtho:
			renderer.camera.SetOrtho();
			renderer.init();
			CheckMenuItem(CameraModeMenu, 0, MF_BYPOSITION | MF_CHECKED);
			CheckMenuItem(CameraModeMenu, 1, MF_BYPOSITION | MF_UNCHECKED);
			break;

		case CMDCameraPersp:
			renderer.camera.SetPerspective();
			renderer.init();
			CheckMenuItem(CameraModeMenu, 1, MF_BYPOSITION | MF_CHECKED);
			CheckMenuItem(CameraModeMenu, 0, MF_BYPOSITION | MF_UNCHECKED);
			break;

		case CMDCameraPos1:
			renderer.camera.SetCameraPosition(points[0]);
			cameraIsFree = false;
			break;

		case CMDCameraPos2:
			renderer.camera.SetCameraPosition(points[1]);
			cameraIsFree = false;
			break;

		case CMDCameraPos3:
			renderer.camera.SetCameraPosition(points[2]);
			cameraIsFree = false;
			break;

		case CMDCameraPos4:
			renderer.camera.SetCameraPosition(points[3]);
			cameraIsFree = false;
			break;

		case CMDCameraPosFree:
			cameraIsFree = true;
			break;

		default: return 0;
		}
		return 0;

	case WM_CLOSE:
		ExitSoftware();
		break;

	case WM_CREATE:
		MainWndAddMenus(hWnd);
		CheckMenuItem(CameraModeMenu, 1, MF_BYPOSITION | MF_CHECKED);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

	}
	break;

	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/**
* \brief main()
* Основная функция программы, ее входная точка
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	if (!MainWndRegisterClass(hInstance, (HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_QUESTION)))
	{
		MessageBox(NULL, "RegisterClass() failed", "Error", MB_ICONERROR | MB_OK);
		return FALSE;
	}
	if (!CreateRenderContext(hInstance, L""))
	{
		MessageBox(NULL, "CreateRenderContext() failed", "Error", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	wglMakeCurrent(hDC, hGLRC);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	renderer.init();

	MSG msg;
	float vel = 0.17f;

	float z_max = 0;
	float z_min = 0;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (cameraIsFree)
		{
			Vector3 position = renderer.camera.GetCameraPosition();
			if (GetAsyncKeyState(0x57) & 0x01) position.x -= vel; //w
			if (GetAsyncKeyState(0x53) & 0x01) position.x += vel; //s
			if (GetAsyncKeyState(0x41) & 0x01) position.z += vel; //a
			if (GetAsyncKeyState(0x44) & 0x01) position.z -= vel; //d
			if (GetAsyncKeyState(VK_UP) & 0x01) position.y += vel;
			if (GetAsyncKeyState(VK_DOWN) & 0x01) position.y -= vel;

			renderer.camera.SetCameraPosition(position);
		}

		renderer.BeginFrame();

		renderer.RenderGrid(-200, 200, 400, -200, 200, 400, 0, Color(50, 50, 50));
		renderer.RenderCootrdinates();

		if (fileIsOpen)
		{
			if (fileIsOpenNow)
			{
				switch (extension)
				{
				case 0:
					scan = OpenObj(fileName);
					break;
				case 1:
					scan = OpenLidar(fileName);
					break;
				}
			}
			scan = FilterPoints(scan);
			z_max = MaxCoord(scan);
			z_min = MinCoord(scan);
			renderer.RenderPicture(scan, z_max, z_min, Color(255, 255, 255));
			fileIsOpenNow = false;
		}

		renderer.EndFrame();
	}

	return (int)msg.wParam;
}
