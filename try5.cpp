#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <string>

// Глобальные переменные
HINSTANCE hInst;
HWND hwndChooseFileButton;
HWND hwndOutput;
HWND hwndExitButton;
HFONT hFont;

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Функция для чтения текста из файла и вывода его в поле программы
void DisplayFileContents(const std::wstring& filePath) {
    std::ifstream file(filePath, std::ios::binary); // Открытие файла в бинарном режиме для корректного чтения UTF-8
    std::string content; // Используем std::string для работы с UTF-8

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        content.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&content[0], content.size());
        file.close();
    }
    else {
        content = "Не удалось открыть файл.";
    }

    // Преобразование текста в символьный массив (string)
    const char* text = content.c_str();

    // Очистка поля вывода перед добавлением нового текста
    SendMessage(hwndOutput, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L""));

    // Вывод содержимого файла в поле программы
    MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    wchar_t* wideText = new wchar_t[size];
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wideText, size);
    SendMessage(hwndOutput, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(wideText));
    delete[] wideText; // Освобождение памяти
}

// Функция для создания и отображения элементов интерфейса
BOOL CreateAndShowWindow(HINSTANCE hInstance, int nCmdShow) {
    HWND hwnd;
    WNDCLASSEX wc;

    // Определение класса окна
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"MyClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Не удалось зарегистрировать класс окна!", L"Ошибка", MB_ICONERROR);
        return FALSE;
    }

    // Создание окна
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"MyClass", L"Программа с файлами",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Не удалось создать окно!", L"Ошибка", MB_ICONERROR);
        return FALSE;
    }

    // Отображение окна
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;

    if (!CreateAndShowWindow(hInstance, nCmdShow)) {
        return FALSE;
    }

    // Цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    switch (msg) {
    case WM_CREATE:
        // Создание кнопки "Выберите файл"
        hwndChooseFileButton = CreateWindowW(L"BUTTON", L"Выберите файл", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, rect.bottom - 40, 120, 30, hwnd, (HMENU)1, NULL, NULL);

        // Создание поля для вывода текста
        hwndOutput = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | ES_WANTRETURN,
            10, 70, rect.right - 20, rect.bottom - 120, hwnd, NULL, NULL, NULL);

        // Установка шрифта Lucida Console для поля вывода
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Lucida Console");
        SendMessage(hwndOutput, WM_SETFONT, WPARAM(hFont), TRUE);

        // Создание кнопки "Выход"
        hwndExitButton = CreateWindowW(L"BUTTON", L"Выход", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            140, rect.bottom - 40, 100, 30, hwnd, (HMENU)2, NULL, NULL);

        // Текст над полем вывода
        CreateWindowW(L"STATIC", L"Привет, пользователь! Для начала работы нажмите на кнопку \"Выберите файл\", для выхода из программы \"Выход\".",
            WS_VISIBLE | WS_CHILD, 10, 10, rect.right - 20, 50, hwnd, NULL, NULL, NULL);

        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Обработка нажатия кнопки "Выберите файл"
            OPENFILENAME ofn;
            wchar_t szFile[MAX_PATH] = L"";

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.hwndOwner = hwnd;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Текстовые файлы\0*.txt\0Все файлы\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Открытие диалогового окна выбора файла
            if (GetOpenFileName(&ofn) == TRUE) {
                std::wstring filePath = ofn.lpstrFile;
                DisplayFileContents(filePath);
            }
        }
        else if (LOWORD(wParam) == 2) { // Обработка нажатия кнопки "Выход"
            DestroyWindow(hwnd);
        }
        break;

    case WM_SIZE:
        // Перерасчет размеров и положения элементов при изменении размеров окна
        GetClientRect(hwnd, &rect);
        MoveWindow(hwndChooseFileButton, 10, rect.bottom - 40, 120, 30, TRUE);
        MoveWindow(hwndOutput, 10, 70, rect.right - 20, rect.bottom - 120, TRUE);
        MoveWindow(hwndExitButton, 140, rect.bottom - 40, 100, 30, TRUE);
        break;


    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}