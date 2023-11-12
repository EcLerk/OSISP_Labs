#pragma once

// Global variables
//#define ID_EDIT 12
#define textBufferSize 256

#define onExit 10
#define onFileLoad 11
#define onFileSave 12
#define onClearClicked 13
#define onReadColor 14

#define DlgIndexColorR 200
#define DlgIndexColorG 201
#define DlgIndexColorB 202

// The main window class name.
char Buffer[textBufferSize];

//mapped file
HANDLE hFile = NULL;
HANDLE hMapping = NULL;
size_t fsize = 0;//размер файла
LPVOID dataPtr = NULL;//указатель на участок памяти с отображением
CONST WCHAR* fileName = L"output.txt";

//PAINTSTRUCT ps;
RECT windowRectangle;
HBRUSH brushRectangle;
HFONT font;

OPENFILENAMEA ofn;
char filename[260];

static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Text Editor");

static HWND hWndEdit;
HWND button;
// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;