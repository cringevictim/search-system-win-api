#include "pch.h" // Підключення заголовкового файлу для попередньо скомпільованих заголовків.
#include <windows.h> // Підключення бібліотеки для роботи з Windows API.
#include <vector> // Підключення бібліотеки для роботи з векторами.
#include <string> // Підключення бібліотеки для роботи з рядками.
#include <algorithm> // Підключення бібліотеки для використання алгоритмів.
#include <sstream> // Підключення бібліотеки для роботи з рядковими потоками.
#include <iomanip> // Підключення бібліотеки для форматування виведення.
#include <fstream> // Підключення бібліотеки для роботи з файлами.
#include <locale> // Підключення бібліотеки для налаштування локалі.
#include <codecvt> // Підключення бібліотеки для конвертації між різними кодуваннями символів.

using namespace std; // Використання стандартного простору імен.

struct Record {
    int id; // Ідентифікатор запису.
    wstring name; // Назва запису.
    wstring type; // Тип запису.
    double quantity; // Кількість.
};

vector<Record> records; // Вектор для зберігання записів.
int nextId = 1; // Наступний доступний ідентифікатор запису.
enum SortType { NAME, TYPE, QUANTITY }; // Перерахування для типів сортування.
SortType currentSortType = NAME; // Поточний тип сортування.

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Оголошення функції обробника віконних повідомлень.
void AddRecord(HWND hwnd); // Оголошення функції додавання запису.
void EditRecord(HWND hwnd); // Оголошення функції редагування запису.
void DeleteRecord(HWND hwnd); // Оголошення функції видалення запису.
void SearchRecords(HWND hwnd); // Оголошення функції пошуку записів.
void SortRecords(); // Оголошення функції сортування записів.
void GenerateReport(HWND hwnd); // Оголошення функції генерації звіту.
void DisplayRecords(HWND hwnd); // Оголошення функції відображення записів.
void LoadData(); // Оголошення функції завантаження даних.
void SaveData(); // Оголошення функції збереження даних.
void ClearFields(HWND hwnd); // Оголошення функції очищення полів вводу.
void UpdateNextId(); // Оголошення функції оновлення наступного доступного ідентифікатора запису.

#define ID_BUTTON_ADD 1 // Ідентифікатор кнопки додавання.
#define ID_BUTTON_EDIT 2 // Ідентифікатор кнопки редагування.
#define ID_BUTTON_DELETE 3 // Ідентифікатор кнопки видалення.
#define ID_BUTTON_SEARCH 4 // Ідентифікатор кнопки пошуку.
#define ID_BUTTON_SORT 5 // Ідентифікатор кнопки сортування.
#define ID_BUTTON_REPORT 6 // Ідентифікатор кнопки генерації звіту.

HWND hName, hType, hQuantity, hResult, hEditId, hDeleteId; // Оголошення дескрипторів елементів інтерфейсу.

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"AgriculturalSearchSystem"; // Ім'я класу вікна.

    WNDCLASS wc = {}; // Створення структури WNDCLASS.

    wc.lpfnWndProc = WindowProc; // Вказання функції обробки повідомлень.
    wc.hInstance = hInstance; // Вказання дескриптора інстанції.
    wc.lpszClassName = CLASS_NAME; // Вказання імені класу.

    RegisterClass(&wc); // Реєстрація класу вікна.

    HWND hwnd = CreateWindowEx(
        0, // Розширений стиль вікна.
        CLASS_NAME, // Ім'я класу.
        L"Agricultural Search System", // Заголовок вікна.
        WS_OVERLAPPEDWINDOW, // Стиль вікна.
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, // Початкові розміри та позиція вікна.
        NULL, // Дескриптор батьківського вікна.
        NULL, // Дескриптор меню.
        hInstance, // Дескриптор інстанції програми.
        NULL // Додаткові параметри.
    );

    if (hwnd == NULL) {
        return 0; // Перевірка на помилку створення вікна.
    }

    ShowWindow(hwnd, nCmdShow); // Відображення вікна.

    MSG msg = {}; // Створення структури повідомлення.
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Перетворення повідомлення.
        DispatchMessage(&msg); // Передача повідомлення функції обробки.
    }

    return 0; // Повернення коду завершення програми.
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Створення елементів інтерфейсу.
        CreateWindow(L"STATIC", L"Name:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hName = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Type:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hwnd, NULL, NULL, NULL);
        hType = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 60, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Quantity:", WS_VISIBLE | WS_CHILD, 20, 100, 100, 20, hwnd, NULL, NULL, NULL);
        hQuantity = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 100, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Edit ID:", WS_VISIBLE | WS_CHILD, 20, 140, 100, 20, hwnd, NULL, NULL, NULL);
        hEditId = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 140, 50, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Delete ID:", WS_VISIBLE | WS_CHILD, 20, 180, 100, 20, hwnd, NULL, NULL, NULL);
        hDeleteId = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 180, 50, 20, hwnd, NULL, NULL, NULL);

        CreateWindow(L"BUTTON", L"Add", WS_VISIBLE | WS_CHILD, 350, 20, 100, 30, hwnd, (HMENU)ID_BUTTON_ADD, NULL, NULL);
        CreateWindow(L"BUTTON", L"Edit", WS_VISIBLE | WS_CHILD, 350, 60, 100, 30, hwnd, (HMENU)ID_BUTTON_EDIT, NULL, NULL);
        CreateWindow(L"BUTTON", L"Delete", WS_VISIBLE | WS_CHILD, 350, 100, 100, 30, hwnd, (HMENU)ID_BUTTON_DELETE, NULL, NULL);
        CreateWindow(L"BUTTON", L"Search", WS_VISIBLE | WS_CHILD, 20, 220, 100, 30, hwnd, (HMENU)ID_BUTTON_SEARCH, NULL, NULL);
        CreateWindow(L"BUTTON", L"Sort", WS_VISIBLE | WS_CHILD, 140, 220, 100, 30, hwnd, (HMENU)ID_BUTTON_SORT, NULL, NULL);
        CreateWindow(L"BUTTON", L"Report", WS_VISIBLE | WS_CHILD, 260, 220, 100, 30, hwnd, (HMENU)ID_BUTTON_REPORT, NULL, NULL);

        hResult = CreateWindow(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL, 20, 260, 540, 180, hwnd, NULL, NULL, NULL);
        LoadData(); // Завантаження даних з файлу.
        SortRecords(); // Сортування записів.
        DisplayRecords(hwnd); // Відображення записів.
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_BUTTON_ADD:
            AddRecord(hwnd); // Виклик функції додавання запису.
            break;
        case ID_BUTTON_EDIT:
            EditRecord(hwnd); // Виклик функції редагування запису.
            break;
        case ID_BUTTON_DELETE:
            DeleteRecord(hwnd); // Виклик функції видалення запису.
            break;
        case ID_BUTTON_SEARCH:
            SearchRecords(hwnd); // Виклик функції пошуку записів.
            break;
        case ID_BUTTON_SORT:
            currentSortType = static_cast<SortType>((currentSortType + 1) % 3); // Зміна типу сортування.
            SortRecords(); // Виклик функції сортування записів.
            DisplayRecords(hwnd); // Відображення відсортованих записів.
            break;
        case ID_BUTTON_REPORT:
            SortRecords(); // Виклик функції сортування записів.
            GenerateReport(hwnd); // Виклик функції генерації звіту.
            break;
        }
        break;
    }
    case WM_DESTROY:
        SaveData(); // Збереження даних у файл.
        PostQuitMessage(0); // Завершення роботи програми.
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); // Виклик стандартної обробки повідомлень.
}

void AddRecord(HWND hwnd) {
    wchar_t name[100], type[100], quantity[100];
    GetWindowText(hName, name, 100); // Отримання значення поля "Name".
    GetWindowText(hType, type, 100); // Отримання значення поля "Type".
    GetWindowText(hQuantity, quantity, 100); // Отримання значення поля "Quantity".

    if (wcslen(name) > 0 && wcslen(type) > 0 && wcslen(quantity) > 0) {
        Record record;
        record.id = nextId; // Присвоєння ідентифікатора новому запису.
        record.name = name; // Присвоєння імені новому запису.
        record.type = type; // Присвоєння типу новому запису.
        record.quantity = _wtof(quantity); // Присвоєння кількості новому запису.

        records.push_back(record); // Додавання запису до вектора.
        ClearFields(hwnd); // Очищення полів вводу.
        UpdateNextId(); // Оновлення наступного доступного ідентифікатора.
        SortRecords(); // Сортування записів.
        DisplayRecords(hwnd); // Відображення записів.
    }
}

void EditRecord(HWND hwnd) {
    wchar_t editIdStr[10], name[100], type[100], quantity[100];
    GetWindowText(hEditId, editIdStr, 10); // Отримання значення поля "Edit ID".
    int editId = _wtoi(editIdStr); // Конвертація значення ID в число.

    GetWindowText(hName, name, 100); // Отримання значення поля "Name".
    GetWindowText(hType, type, 100); // Отримання значення поля "Type".
    GetWindowText(hQuantity, quantity, 100); // Отримання значення поля "Quantity".

    for (auto& record : records) {
        if (record.id == editId) { // Пошук запису за ID.
            record.name = name; // Оновлення імені запису.
            record.type = type; // Оновлення типу запису.
            record.quantity = _wtof(quantity); // Оновлення кількості запису.
            ClearFields(hwnd); // Очищення полів вводу.
            SortRecords(); // Сортування записів.
            DisplayRecords(hwnd); // Відображення записів.
            return;
        }
    }
}

void DeleteRecord(HWND hwnd) {
    wchar_t deleteIdStr[10];
    GetWindowText(hDeleteId, deleteIdStr, 10); // Отримання значення поля "Delete ID".
    int deleteId = _wtoi(deleteIdStr); // Конвертація значення ID в число.

    records.erase(remove_if(records.begin(), records.end(), [deleteId](Record& record) {
        return record.id == deleteId; // Видалення запису з вектора за ID.
        }), records.end());

    ClearFields(hwnd); // Очищення полів вводу.
    UpdateNextId(); // Оновлення наступного доступного ідентифікатора.
    SortRecords(); // Сортування записів.
    DisplayRecords(hwnd); // Відображення записів.
}

void SearchRecords(HWND hwnd) {
    wchar_t name[100], type[100], quantity[100];
    GetWindowText(hName, name, 100); // Отримання значення поля "Name".
    GetWindowText(hType, type, 100); // Отримання значення поля "Type".
    GetWindowText(hQuantity, quantity, 100); // Отримання значення поля "Quantity".

    double q = _wtof(quantity); // Конвертація значення кількості в число.
    wstring result = L""; // Ініціалізація рядка для результатів пошуку.
    for (const auto& record : records) {
        if ((wcslen(name) == 0 || record.name == name) &&
            (wcslen(type) == 0 || record.type == type) &&
            (wcslen(quantity) == 0 || record.quantity == q)) {
            wostringstream oss;
            oss << L"ID: " << record.id << L", Name: " << record.name << L", Type: " << record.type << L", Quantity: " << fixed << setprecision(2) << record.quantity << L"\r\n";
            result += oss.str(); // Додавання результатів пошуку до рядка.
        }
    }
    SetWindowText(hResult, result.c_str()); // Відображення результатів пошуку.
}

void SortRecords() {
    switch (currentSortType) {
    case NAME:
        sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
            return a.name < b.name; // Сортування за ім'ям.
            });
        break;
    case TYPE:
        sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
            return a.type < b.type; // Сортування за типом.
            });
        break;
    case QUANTITY:
        sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
            return a.quantity < b.quantity; // Сортування за кількістю.
            });
        break;
    }
}

void GenerateReport(HWND hwnd) {
    wstring report = L"ID\tName\tType\tQuantity\r\n"; // Заголовок звіту.
    for (const auto& record : records) {
        wostringstream oss;
        oss << record.id << L'\t' << record.name << L'\t' << record.type << L'\t' << fixed << setprecision(2) << record.quantity << L'\n';
        report += oss.str(); // Додавання записів до звіту.
    }
    SetWindowText(hResult, report.c_str()); // Відображення звіту.
}

void DisplayRecords(HWND hwnd) {
    wstring result = L""; // Ініціалізація рядка для відображення записів.
    for (const auto& record : records) {
        wostringstream oss;
        oss << L"ID: " << record.id << L", Name: " << record.name << L", Type: " << record.type << L", Quantity: " << fixed << setprecision(2) << record.quantity << L"\r\n";
        result += oss.str(); // Додавання записів до рядка.
    }
    SetWindowText(hResult, result.c_str()); // Відображення записів.
}

void ClearFields(HWND hwnd) {
    SetWindowText(hName, L""); // Очищення поля "Name".
    SetWindowText(hType, L""); // Очищення поля "Type".
    SetWindowText(hQuantity, L""); // Очищення поля "Quantity".
    SetWindowText(hEditId, L""); // Очищення поля "Edit ID".
    SetWindowText(hDeleteId, L""); // Очищення поля "Delete ID".
}

void LoadData() {
    wifstream infile("data.txt"); // Відкриття файлу для читання.
    infile.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>)); // Налаштування локалі для UTF-8.

    if (!infile) return; // Перевірка на помилку відкриття файлу.

    records.clear(); // Очищення вектора записів.
    nextId = 1; // Ініціалізація наступного доступного ідентифікатора.
    Record record;
    wstring line;
    while (getline(infile, line)) { // Читання рядків з файлу.
        wistringstream iss(line);
        iss >> record.id; // Читання ідентифікатора.
        iss.ignore(1, L'\t'); // Ігнорування символу табуляції.
        getline(iss, record.name, L'\t'); // Читання імені.
        getline(iss, record.type, L'\t'); // Читання типу.
        iss >> record.quantity; // Читання кількості.
        records.push_back(record); // Додавання запису до вектора.
    }
    UpdateNextId(); // Оновлення наступного доступного ідентифікатора.
    SortRecords(); // Сортування записів.
}

void SaveData() {
    wofstream outfile("data.txt"); // Відкриття файлу для запису.
    outfile.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>)); // Налаштування локалі для UTF-8.

    for (const auto& record : records) {
        outfile << record.id << L'\t' << record.name << L'\t' << record.type << L'\t' << fixed << setprecision(2) << record.quantity << L'\n'; // Запис запису у файл.
    }
}

void UpdateNextId() {
    nextId = 1; // Ініціалізація наступного доступного ідентифікатора.
    for (const auto& record : records) {
        if (record.id >= nextId) {
            nextId = record.id + 1; // Оновлення наступного доступного ідентифікатора на основі максимального ID.
        }
    }
}
