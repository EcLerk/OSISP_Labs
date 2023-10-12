#include <windows.h>
#include <tchar.h>
#include "main.h"


// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void AddWidget(HWND hWnd);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = NULL;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop "),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

   // AddWidget(hWnd);
    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void AddMenu(HWND hWnd) 
{
    HMENU RootMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();

    AppendMenu(SubMenu, MF_STRING, onFileSave, L"Save");
    AppendMenu(SubMenu, MF_STRING, onFileLoad, L"Open file");
    AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(SubMenu, MF_STRING, onExit, L"Exit");

    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");

    SetMenu(hWnd, RootMenu);
}

void SaveFile(LPCSTR path, HWND hWndEdit)
{
    HANDLE fileToSave = CreateFileA(
        path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    int saveLength = GetWindowTextLength(hWndEdit) + 1;
    char* data = new char[saveLength];

    saveLength = GetWindowTextA(hWndEdit, data, saveLength);

    DWORD bytesIterated;
    WriteFile(fileToSave, data, saveLength, &bytesIterated, NULL);

    CloseHandle(fileToSave);
    delete[] data;
}

void LoadFile(LPCSTR path, HWND hWndEdit)
{
    HANDLE fileToLoad = CreateFileA(
        path,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    DWORD bytesIterated;
    ReadFile(fileToLoad, Buffer, textBufferSize, &bytesIterated, NULL);

    SetWindowTextA(hWndEdit, Buffer);
    memset(Buffer, 0, 100);
    CloseHandle(fileToLoad);
}

void SetOpenFileParams(HWND hWnd)
{
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = ".txt";
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = "E:/MyEditor_Saved";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

}

void AddWidget(HWND hWnd)
{
    // Creating a editfield for a text
    hWndEdit = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 5, 5, 470, 400, hWnd, (HMENU)1, NULL, NULL);
    windowRectangle = { 500, 0, 0, 600 };

    CreateWindowA("edit", "0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER | WS_BORDER , 5, 445, 100, 20, hWnd, HMENU(DlgIndexColorR), NULL, NULL);
    CreateWindowA("edit", "0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER | WS_BORDER, 120, 445, 100, 20, hWnd, HMENU(DlgIndexColorB), NULL, NULL);
    CreateWindowA("edit", "0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER | WS_BORDER, 240, 445, 100, 20, hWnd, HMENU(DlgIndexColorG), NULL, NULL);

    CreateWindowA("button", "Clear", WS_VISIBLE | WS_CHILD,
       5, 410, 100, 25, // Размер и позиция кнопки под текстовым полем
       hWnd, (HMENU)onClearClicked, NULL, NULL);
    CreateWindowA("button", "Set color", WS_VISIBLE | WS_CHILD, 115, 410, 100, 25, hWnd, (HMENU)onReadColor, NULL, NULL);
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:

       AddWidget(hWnd);
       AddMenu(hWnd);
        
        SetOpenFileParams(hWnd);
        font = CreateFontA(26, 10, 0, 0, FW_MEDIUM, 
            TRUE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
            CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            FF_ROMAN, "MyFont");

        SendMessage(hWndEdit, WM_SETFONT, WPARAM(font), TRUE);
        return 0;

    case WM_PAINT:
    {
        //PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);

        // Setting the background color to light blue (RGB(153, 204, 255))
        HBRUSH hLightBlueBrush = CreateSolidBrush(RGB(153, 204, 255));
        FillRect(hdc, &rc, hLightBlueBrush);
        DeleteObject(hLightBlueBrush);

        FillRect(ps.hdc, &windowRectangle, brushRectangle);
        // Main Background the same as the system
        //FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hWnd, &ps);
    }
    break;

    //case WM_SETFOCUS:
    //    SetFocus(hWndEdit);
    //    return 0;

    //case WM_SIZE:
    //    MoveWindow(hWndEdit, 0, 0, LOWORD(lParam),
    //        HIWORD(lParam), TRUE);
    //    return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case onFileLoad:
            if (GetOpenFileNameA(&ofn))
            {
                LoadFile(filename, hWndEdit);
            }
            return 0;
        case onFileSave:
            if (GetSaveFileNameA(&ofn)) {
                SaveFile(filename, hWndEdit);
            }
            
            return 0;
        case onExit:
            PostQuitMessage(0);
            return 0;
        case onClearClicked:
            SetWindowTextA(hWndEdit, "");
            return 0;

        case onReadColor:

            brushRectangle = CreateSolidBrush(
                RGB(GetDlgItemInt(hWnd, DlgIndexColorR, FALSE, false),
                    GetDlgItemInt(hWnd, DlgIndexColorG, FALSE, false),
                    GetDlgItemInt(hWnd, DlgIndexColorB, FALSE, false))
            );

            RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
            return 0;

        case 1:
            if (HIWORD(wParam) == EN_ERRSPACE ||
                HIWORD(wParam) == EN_MAXTEXT)
                MessageBox(hWnd, L"Edit control out of space.",
                    szTitle, MB_OK | MB_ICONSTOP);
            return 0;
        default:
            break;

        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

  