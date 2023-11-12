#include "main.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void MonitorVolumeChanges();
void ResizeRectangle(RECT& rect, int newHeight);
void OnVolumeChange(float);
void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);
void SaveColorToRegistry(COLORREF);
bool ReadColorFromRegistry(COLORREF&);
bool ReadVolumeFromRegistry(float& volume);
void RestoreColorAndVolumeFromRegistry();

HWND hWnd; 
COLORREF currentColor = RGB(178, 132, 190);
COLORREF targetColor = RGB(178, 132, 190);
const int timerInterval = 100; 
int currentHeight;
RECT rect = { 0, 0, 140, 0 };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground = CreateSolidBrush(RGB(178, 132, 190));
    wcex.lpszClassName = L"Volume Monitor";
    RegisterClassEx(&wcex);

    hWnd = CreateWindow(L"Volume Monitor", L"Volume Monitor", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 100, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return false;
    }

    RestoreColorAndVolumeFromRegistry();

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    
    std::thread monitorThread(MonitorVolumeChanges);

    
    SetTimer(hWnd, 1, timerInterval, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    
    KillTimer(hWnd, 1); 
    monitorThread.detach();
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Рисование фона
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        // Рисование прямоугольника с текущей высотой
        HBRUSH rectColor = CreateSolidBrush(targetColor);
        FillRect(hdc, &rect, rectColor);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_TIMER:
        TimerProc(hWnd, WM_TIMER, 1, GetTickCount64());
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnVolumeChange(float newVolume) {
   
    static bool firstChange = true;
    if (firstChange) {
        COLORREF savedColor;
        if (ReadColorFromRegistry(savedColor)) {
            currentColor = savedColor;
            targetColor = savedColor;
        }
        firstChange = false;
    }

    int newHeight = 560 * newVolume;
    ResizeRectangle(rect, newHeight);
    //InvalidateRect(hWnd, NULL, TRUE);

    int green = static_cast<int>(255 * newVolume);
    targetColor = RGB(0, green, 0);

    SetTimer(hWnd, 1, 100, nullptr);

    SaveColorToRegistry(targetColor);
}

void MonitorVolumeChanges() {
    CoInitialize(nullptr);

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));

    if (SUCCEEDED(hr)) {
        IMMDevice* defaultDevice = nullptr;
        hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

        if (SUCCEEDED(hr)) {
            IAudioEndpointVolume* endpointVolume = nullptr;
            hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&endpointVolume));

            if (SUCCEEDED(hr)) {
                while (true) {
                    float currentVolume = 0.0f;
                    hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

                    if (SUCCEEDED(hr)) {
                        OnVolumeChange(currentVolume);
                    }

                    Sleep(1000); 
                }
                endpointVolume->Release();
            }
            defaultDevice->Release();
        }
        enumerator->Release();
    }
    CoUninitialize();
}

void ResizeRectangle(RECT& rect, int newHeight)
{
    // Изменение размера прямоугольника на новые значения
    rect.bottom = rect.top + newHeight;
    currentHeight = newHeight;
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    
    int rDiff = (GetRValue(targetColor) - GetRValue(currentColor)) / 10;
    int gDiff = (GetGValue(targetColor) - GetGValue(currentColor)) / 10;
    int bDiff = (GetBValue(targetColor) - GetBValue(currentColor)) / 10;

    currentColor = RGB(GetRValue(currentColor) + rDiff,
        GetGValue(currentColor) + gDiff,
        GetBValue(currentColor) + bDiff);

    HBRUSH hBrush = CreateSolidBrush(currentColor);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(hBrush));
    InvalidateRect(hwnd, nullptr, TRUE);

    if (currentColor == targetColor) {
        KillTimer(hwnd, 1);
    }
}

void SaveColorToRegistry(COLORREF color) {
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        DWORD dwData = color;
        RegSetValueEx(hKey, L"WindowColor", 0, REG_DWORD, (BYTE*)&dwData, sizeof(dwData));
        RegCloseKey(hKey);
    }
}

bool ReadColorFromRegistry(COLORREF& color) {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwData;
        DWORD dwSize = sizeof(DWORD);

        if (RegQueryValueEx(hKey, L"WindowColor", nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwData), &dwSize) == ERROR_SUCCESS) {
            color = static_cast<COLORREF>(dwData);
            RegCloseKey(hKey);
            return true;
        }

        RegCloseKey(hKey);
    }
    return false;
}

bool ReadVolumeFromRegistry(float& volume) {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwData;
        DWORD dwSize = sizeof(DWORD);

        if (RegQueryValueEx(hKey, L"VolumeLevel", nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwData), &dwSize) == ERROR_SUCCESS) {
            volume = static_cast<float>(dwData) / 100.0f; // Преобразование из DWORD в float
            RegCloseKey(hKey);
            return true;
        }

        RegCloseKey(hKey);
    }
    return false;
}


void RestoreColorAndVolumeFromRegistry() {
    COLORREF restoredColor;
    if (ReadColorFromRegistry(restoredColor)) {

        HBRUSH hBrush = CreateSolidBrush(restoredColor);
        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(hBrush));
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    float restoredVolume = 0.5f;
    if (ReadVolumeFromRegistry(restoredVolume)) {
        OnVolumeChange(restoredVolume);
    }
}