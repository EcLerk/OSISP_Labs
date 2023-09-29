#include <windows.h>
#include <tchar.h>

// Global variables
//#define ID_EDIT 12
#define textBufferSize 256

#define onExit 10
#define onFileLoad 11
#define onFileSave 12
#define onClearClicked 13

// The main window class name.
char Buffer[textBufferSize];

OPENFILENAMEA ofn;
char filename[260];

static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Text Editor");

static HWND hWndEdit;
// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

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
            _T("Windows Desktop Guided Tour"),
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
   // CreateWindowA("static", "some window", WS_VISIBLE | WS_CHILD, 5, 5, 500, 20, hWnd, NULL, NULL, NULL);
  /*  hWndEdit = CreateWindowA("edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
        WS_BORDER | ES_LEFT | ES_MULTILINE |
        ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        0, 0, 100, 100,
        hWnd, NULL,
        NULL, NULL);*/
        // Creating a editfield for a text
    hWndEdit = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 5, 5, 470, 400, hWnd, (HMENU)1, NULL, NULL);
    //hWndEdit = CreateWindowA("edit", NULL,
    //    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE,
    //    0, 0, 100, 100, // Здесь устанавливаем размер текстового поля 100x100
    //    hWnd, NULL,
    //    NULL, NULL);

   CreateWindowA("button", "Clear", WS_VISIBLE | WS_CHILD,
       5, 410, 100, 25, // Размер и позиция кнопки под текстовым полем
       hWnd, (HMENU)onClearClicked, NULL, NULL);
    //hWndEdit = CreateWindowA("edit", NULL,
    //    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE,
    //    0, 0, 100, 100, // Здесь устанавливаем фиксированный размер 100x100
    //    hWnd, NULL,
    //    NULL, NULL);
    //CreateWindowA("button", "Click me!", WS_VISIBLE | WS_CHILD | ES_CENTER, 5 ,420,120, 20, hWnd, NULL, NULL, NULL);
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
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);

        // Setting the background color to light blue (RGB(153, 204, 255))
        HBRUSH hLightBlueBrush = CreateSolidBrush(RGB(153, 204, 255));
        FillRect(hdc, &rc, hLightBlueBrush);
        DeleteObject(hLightBlueBrush);


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

  