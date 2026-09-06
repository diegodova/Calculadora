#include "Calculadora.h"

#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>

// ============================================================
// IDs de los controles
// ============================================================

#define ID_DISPLAY 100

#define ID_0       200
#define ID_1       201
#define ID_2       202
#define ID_3       203
#define ID_4       204
#define ID_5       205
#define ID_6       206
#define ID_7       207
#define ID_8       208
#define ID_9       209

#define ID_ADD     210
#define ID_SUB     211
#define ID_MUL     212
#define ID_DIV     213
#define ID_EQUAL   214

#define ID_DOT     215
#define ID_CLEAR   216
#define ID_BACK    217
#define ID_SIGN    218

// ============================================================
// Variables de la calculadora
// ============================================================

HWND hDisplay = nullptr;

double storedValue = 0.0;
wchar_t pendingOperator = L'\0';

bool newInput = true;

// ============================================================
// Utilidades
// ============================================================

void SetDisplay(const std::wstring& text)
{
    SetWindowTextW(hDisplay, text.c_str());
}

std::wstring GetDisplay()
{
    wchar_t buffer[256]{};

    GetWindowTextW(
        hDisplay,
        buffer,
        static_cast<int>(std::size(buffer))
    );

    return buffer;
}

double GetDisplayValue()
{
    try
    {
        return std::stod(GetDisplay());
    }
    catch (...)
    {
        return 0.0;
    }
}

std::wstring NumberToString(double value)
{
    std::wostringstream stream;

    stream << std::setprecision(15) << value;

    return stream.str();
}

// ============================================================
// Operaciones
// ============================================================

double Calculate(double a, double b, wchar_t operation)
{
    switch (operation)
    {
    case L'+':
        return a + b;

    case L'-':
        return a - b;

    case L'*':
        return a * b;

    case L'/':
        if (b == 0.0)
        {
            MessageBoxW(
                nullptr,
                L"No se puede dividir entre cero.",
                L"Error",
                MB_OK | MB_ICONERROR
            );

            return 0.0;
        }

        return a / b;
    }

    return b;
}

// ============================================================
// Procesar números
// ============================================================

void PressNumber(int number)
{
    std::wstring current = GetDisplay();

    if (newInput)
    {
        current = std::to_wstring(number);
        newInput = false;
    }
    else
    {
        if (current == L"0")
            current = std::to_wstring(number);
        else
            current += std::to_wstring(number);
    }

    SetDisplay(current);
}

void PressDecimal()
{
    if (newInput)
    {
        SetDisplay(L"0.");
        newInput = false;
        return;
    }

    std::wstring current = GetDisplay();

    if (current.find(L'.') == std::wstring::npos)
    {
        current += L'.';
        SetDisplay(current);
    }
}

// ============================================================
// Procesar operadores
// ============================================================

void PressOperator(wchar_t operation)
{
    double currentValue = GetDisplayValue();

    if (pendingOperator != L'\0' && !newInput)
    {
        storedValue = Calculate(
            storedValue,
            currentValue,
            pendingOperator
        );

        SetDisplay(NumberToString(storedValue));
    }
    else
    {
        storedValue = currentValue;
    }

    pendingOperator = operation;
    newInput = true;
}

void PressEquals()
{
    if (pendingOperator == L'\0')
        return;

    double currentValue = GetDisplayValue();

    double result = Calculate(
        storedValue,
        currentValue,
        pendingOperator
    );

    SetDisplay(NumberToString(result));

    storedValue = result;
    pendingOperator = L'\0';
    newInput = true;
}

// ============================================================
// Crear botón
// ============================================================

HWND CreateButton(
    HWND parent,
    const wchar_t* text,
    int id,
    int x,
    int y,
    int width,
    int height
)
{
    return CreateWindowW(
        L"BUTTON",
        text,
        WS_VISIBLE |
        WS_CHILD |
        BS_PUSHBUTTON,
        x,
        y,
        width,
        height,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr),
        nullptr
    );
}

// ============================================================
// Crear controles
// ============================================================

void CreateControls(HWND hwnd)
{
    // --------------------------------------------------------
    // Display
    // --------------------------------------------------------

    hDisplay = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"0",
        WS_VISIBLE |
        WS_CHILD |
        ES_RIGHT |
        ES_READONLY,
        20,
        20,
        280,
        55,
        hwnd,
        reinterpret_cast<HMENU>(ID_DISPLAY),
        GetModuleHandleW(nullptr),
        nullptr
    );

    // Fuente del display
    HFONT displayFont = CreateFontW(
        28,
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );

    SendMessageW(
        hDisplay,
        WM_SETFONT,
        reinterpret_cast<WPARAM>(displayFont),
        TRUE
    );

    // --------------------------------------------------------
    // Botones
    // --------------------------------------------------------

    const int startX = 20;
    const int startY = 90;

    const int buttonWidth = 65;
    const int buttonHeight = 50;

    const int gap = 5;

    auto X = [&](int column)
        {
            return startX + column * (buttonWidth + gap);
        };

    auto Y = [&](int row)
        {
            return startY + row * (buttonHeight + gap);
        };

    // Fila 0
    CreateButton(hwnd, L"C", ID_CLEAR, X(0), Y(0), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"⌫", ID_BACK, X(1), Y(0), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"±", ID_SIGN, X(2), Y(0), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"÷", ID_DIV, X(3), Y(0), buttonWidth, buttonHeight);

    // Fila 1
    CreateButton(hwnd, L"7", ID_7, X(0), Y(1), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"8", ID_8, X(1), Y(1), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"9", ID_9, X(2), Y(1), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"×", ID_MUL, X(3), Y(1), buttonWidth, buttonHeight);

    // Fila 2
    CreateButton(hwnd, L"4", ID_4, X(0), Y(2), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"5", ID_5, X(1), Y(2), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"6", ID_6, X(2), Y(2), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"-", ID_SUB, X(3), Y(2), buttonWidth, buttonHeight);

    // Fila 3
    CreateButton(hwnd, L"1", ID_1, X(0), Y(3), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"2", ID_2, X(1), Y(3), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"3", ID_3, X(2), Y(3), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"+", ID_ADD, X(3), Y(3), buttonWidth, buttonHeight);

    // Fila 4
    CreateButton(hwnd, L"0", ID_0, X(0), Y(4), buttonWidth * 2 + gap, buttonHeight);
    CreateButton(hwnd, L".", ID_DOT, X(2), Y(4), buttonWidth, buttonHeight);
    CreateButton(hwnd, L"=", ID_EQUAL, X(3), Y(4), buttonWidth, buttonHeight);
}

// ============================================================
// WindowProc
// ============================================================

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
    case WM_CREATE:
        CreateControls(hwnd);
        return 0;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);

        switch (id)
        {
        case ID_0: PressNumber(0); break;
        case ID_1: PressNumber(1); break;
        case ID_2: PressNumber(2); break;
        case ID_3: PressNumber(3); break;
        case ID_4: PressNumber(4); break;
        case ID_5: PressNumber(5); break;
        case ID_6: PressNumber(6); break;
        case ID_7: PressNumber(7); break;
        case ID_8: PressNumber(8); break;
        case ID_9: PressNumber(9); break;

        case ID_ADD:
            PressOperator(L'+');
            break;

        case ID_SUB:
            PressOperator(L'-');
            break;

        case ID_MUL:
            PressOperator(L'*');
            break;

        case ID_DIV:
            PressOperator(L'/');
            break;

        case ID_EQUAL:
            PressEquals();
            break;

        case ID_DOT:
            PressDecimal();
            break;

        case ID_CLEAR:
            SetDisplay(L"0");
            storedValue = 0.0;
            pendingOperator = L'\0';
            newInput = true;
            break;

        case ID_BACK:
        {
            if (!newInput)
            {
                std::wstring value = GetDisplay();

                if (value.length() > 1)
                {
                    value.pop_back();
                }
                else
                {
                    value = L"0";
                    newInput = true;
                }

                SetDisplay(value);
            }

            break;
        }

        case ID_SIGN:
        {
            double value = GetDisplayValue();
            value *= -1;

            SetDisplay(NumberToString(value));

            break;
        }
        }

        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(
        hwnd,
        uMsg,
        wParam,
        lParam
    );
}

// ============================================================
// WinMain
// ============================================================

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    PWSTR,
    int nCmdShow
)
{
    const wchar_t CLASS_NAME[] = L"CalculadoraWin32";

    WNDCLASSW wc{};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(
        COLOR_WINDOW + 1
        );

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Calculadora",
        WS_OVERLAPPED |
        WS_CAPTION |
        WS_SYSMENU |
        WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        340,
        440,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};

    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
