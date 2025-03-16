#include <windows.h>
#include <iostream>
#include <string>
#include <functional>
#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <limits>

using namespace std;

void SetConsoleToUTF8() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stdin),  _O_U8TEXT);
}

void WaitForKeyPress() {
    wcout << L"\n(Нажмите любую клавишу для возврата в меню)";
    _getch();
}

void ClearScreen() {
    system("cls");
}

void PrintFileSystemFlags(DWORD flags) {
    if (flags & FILE_CASE_SENSITIVE_SEARCH)
        wcout << L"  - FILE_CASE_SENSITIVE_SEARCH (Поиск с учётом регистра)\n";
    if (flags & FILE_CASE_PRESERVED_NAMES)
        wcout << L"  - FILE_CASE_PRESERVED_NAMES (Сохраняет регистр имён)\n";
    if (flags & FILE_FILE_COMPRESSION)
        wcout << L"  - FILE_FILE_COMPRESSION (Сжатие файлов)\n";
    if (flags & FILE_SUPPORTS_ENCRYPTION)
        wcout << L"  - FILE_SUPPORTS_ENCRYPTION (Шифрование файлов)\n";
    if (flags & FILE_SUPPORTS_SPARSE_FILES)
        wcout << L"  - FILE_SUPPORTS_SPARSE_FILES (Разреженные файлы)\n";
    if (flags & FILE_SUPPORTS_REPARSE_POINTS)
        wcout << L"  - FILE_SUPPORTS_REPARSE_POINTS (Символические ссылки, junctions)\n";
    if (flags & FILE_VOLUME_IS_COMPRESSED)
        wcout << L"  - FILE_VOLUME_IS_COMPRESSED (Том сжат)\n";
    if (flags & FILE_READ_ONLY_VOLUME)
        wcout << L"  - FILE_READ_ONLY_VOLUME (Только для чтения)\n";
    if (flags & FILE_SUPPORTS_HARD_LINKS)
        wcout << L"  - FILE_SUPPORTS_HARD_LINKS (Жёсткие ссылки)\n";
    if (flags & FILE_SUPPORTS_TRANSACTIONS)
        wcout << L"  - FILE_SUPPORTS_TRANSACTIONS (Транзакции)\n";
    if (flags & FILE_SUPPORTS_OBJECT_IDS)
        wcout << L"  - FILE_SUPPORTS_OBJECT_IDS (Уникальные ID файлов)\n";
    if (flags & FILE_SUPPORTS_USN_JOURNAL)
        wcout << L"  - FILE_SUPPORTS_USN_JOURNAL (Журнал изменений)\n";
    if (flags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
        wcout << L"  - FILE_SUPPORTS_OPEN_BY_FILE_ID (Открытие по ID файла)\n";
    if (flags & FILE_SUPPORTS_INTEGRITY_STREAMS)
        wcout << L"  - FILE_SUPPORTS_INTEGRITY_STREAMS (Контроль целостности, ReFS)\n";
    if (flags & FILE_SUPPORTS_BLOCK_REFCOUNTING)
        wcout << L"  - FILE_SUPPORTS_BLOCK_REFCOUNTING (Экономия места, ReFS)\n";
    if (flags & FILE_SUPPORTS_SPARSE_VDL)
        wcout << L"  - FILE_SUPPORTS_SPARSE_VDL (Оптимизированные sparse, ReFS)\n";

    DWORD knownFlags =
            FILE_CASE_SENSITIVE_SEARCH | FILE_CASE_PRESERVED_NAMES
            | FILE_FILE_COMPRESSION | FILE_SUPPORTS_ENCRYPTION
            | FILE_SUPPORTS_SPARSE_FILES | FILE_SUPPORTS_REPARSE_POINTS
            | FILE_VOLUME_IS_COMPRESSED | FILE_READ_ONLY_VOLUME
            | FILE_SUPPORTS_HARD_LINKS | FILE_SUPPORTS_TRANSACTIONS
            | FILE_SUPPORTS_OBJECT_IDS | FILE_SUPPORTS_USN_JOURNAL
            | FILE_SUPPORTS_OPEN_BY_FILE_ID | FILE_SUPPORTS_INTEGRITY_STREAMS
            | FILE_SUPPORTS_BLOCK_REFCOUNTING | FILE_SUPPORTS_SPARSE_VDL;

    DWORD unknown = flags & ~knownFlags;
    if (unknown != 0) {
        wcout << L"  - Доп. флаги: 0x" << std::hex << unknown << std::dec << endl;
    }
}

void ListDrives() {
    DWORD bufSize = GetLogicalDriveStringsW(0, nullptr);
    if (bufSize == 0) {
        wcerr << L"Не удалось получить список дисков!" << endl;
        return;
    }
    auto* buffer = new wchar_t[bufSize];
    DWORD result = GetLogicalDriveStringsW(bufSize, buffer);
    if (result == 0) {
        wcerr << L"Ошибка при получении списка дисков!" << endl;
        delete[] buffer;
        return;
    }
    wcout << L"--- Доступные диски ---" << endl;
    wchar_t* current = buffer;
    while (*current) {
        wcout << current << endl;
        current += wcslen(current) + 1;
    }
    delete[] buffer;
}

void ShowDriveInfo() {
    wstring drive;
    wcout << L"Введите букву диска (например, C): ";
    wcin >> drive;

    if (drive.size() == 1) {
        drive += L":\\";
    } else if (drive.size() == 2 && drive[1] == L':') {
        drive += L"\\";
    }

    UINT driveType = GetDriveTypeW(drive.c_str());
    wcout << L"\nТип диска: ";
    switch (driveType) {
        case DRIVE_UNKNOWN:      wcout << L"Неизвестный"; break;
        case DRIVE_NO_ROOT_DIR:  wcout << L"Нет корневого каталога"; break;
        case DRIVE_REMOVABLE:    wcout << L"Съёмный диск"; break;
        case DRIVE_FIXED:        wcout << L"Жёсткий диск (Fixed)"; break;
        case DRIVE_REMOTE:       wcout << L"Сетевой диск"; break;
        case DRIVE_CDROM:        wcout << L"CD/DVD диск"; break;
        case DRIVE_RAMDISK:      wcout << L"RAM диск"; break;
        default:                 wcout << L"Неизвестный"; break;
    }
    wcout << endl;

    wchar_t volName[MAX_PATH] = {0};
    wchar_t fsName[MAX_PATH] = {0};
    DWORD serialNumber = 0, maxLen = 0, fsFlags = 0;
    if (GetVolumeInformationW(
            drive.c_str(),
            volName, MAX_PATH,
            &serialNumber,
            &maxLen,
            &fsFlags,
            fsName, MAX_PATH))
    {
        wcout << L"Метка тома: " << (volName[0] ? volName : L"(нет)") << endl;
        wcout << L"Файловая система: " << (fsName[0] ? fsName : L"(нет)") << endl;
        wcout << L"Серийный номер: " << serialNumber << endl;
        wcout << L"Макс. длина имени файла: " << maxLen << endl;

        wcout << L"\nСистемные флаги файловой системы:\n";
        PrintFileSystemFlags(fsFlags);

    } else {
        wcerr << L"Ошибка: не удалось получить информацию о томе." << endl;
    }

    DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
    if (GetDiskFreeSpaceW(drive.c_str(),
                          &sectorsPerCluster,
                          &bytesPerSector,
                          &freeClusters,
                          &totalClusters)) {
        ULONGLONG freeBytes =
                (ULONGLONG) sectorsPerCluster * bytesPerSector * freeClusters;
        ULONGLONG totalBytes =
                (ULONGLONG) sectorsPerCluster * bytesPerSector * totalClusters;
        wcout << L"\nСвободно: " << freeBytes << L" байт" << endl;
        wcout << L"Всего на диске: " << totalBytes << L" байт" << endl;
    } else {
        wcerr << L"Не удалось узнать свободное место." << endl;
    }
}

void CreateDirectory() {
    wcin.ignore();
    wstring dirPath;
    wcout << L"Введите путь для нового каталога: ";
    getline(wcin, dirPath);

    if (CreateDirectoryW(dirPath.c_str(), nullptr)) {
        wcout << L"Каталог создан: " << dirPath << endl;
    } else {
        wcerr << L"Ошибка создания каталога. Код: " << GetLastError() << endl;
    }
}

void RemoveDirectory() {
    wcin.ignore();
    wstring rootPath;
    wcout << L"Введите путь для удаления каталога: ";
    getline(wcin, rootPath);

    DWORD fa = GetFileAttributesW(rootPath.c_str());
    if (fa == INVALID_FILE_ATTRIBUTES || !(fa & FILE_ATTRIBUTE_DIRECTORY)) {
        wcerr << L"Ошибка: Каталог не найден.\n";
        return;
    }

    std::function<void(const wstring&)> removeAll = [&](const wstring& dir) {
        WIN32_FIND_DATAW fd;
        wstring sp = dir + L"\\*";
        HANDLE h = FindFirstFileW(sp.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                wstring name = fd.cFileName;
                if (name == L"." || name == L"..") continue;
                wstring full = dir + L"\\" + name;

                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    removeAll(full);
                } else {
                    if (!DeleteFileW(full.c_str())) {
                        wcerr << L"Ошибка удаления файла: " << full
                              << L". Код: " << GetLastError() << endl;
                    }
                }
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }

        SetCurrentDirectoryW(L"C:\\");
        if (!RemoveDirectoryW(dir.c_str())) {
            wcerr << L"Ошибка удаления каталога. Код: " << GetLastError() << endl;
        } else {
            wcout << L"Каталог удалён: " << dir << endl;
        }
    };
    removeAll(rootPath);
}

void CreateFile() {
    wcin.ignore();
    wstring filePath;
    wcout << L"Введите полный путь для создания файла: ";
    getline(wcin, filePath);

    HANDLE hFile = CreateFileW(filePath.c_str(),
                               GENERIC_WRITE,
                               0,
                               nullptr,
                               CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        wcerr << L"Ошибка создания файла. Код: " << GetLastError() << endl;
    } else {
        wcout << L"Файл создан: " << filePath << endl;
        CloseHandle(hFile);
    }
}

void CopyFile() {
    wcin.ignore();
    wstring src, dst;
    wcout << L"Путь исходного файла: ";
    getline(wcin, src);
    wcout << L"Путь для копии файла: ";
    getline(wcin, dst);

    if (CopyFileW(src.c_str(), dst.c_str(), FALSE)) {
        wcout << L"Файл успешно скопирован в " << dst << endl;
    } else {
        wcerr << L"Ошибка копирования файла. Код: " << GetLastError() << endl;
    }
}

void MoveFile() {
    wcin.ignore();
    wstring src, dst;
    wcout << L"Исходный файл: ";
    getline(wcin, src);
    wcout << L"Новый путь/имя: ";
    getline(wcin, dst);

    if (MoveFileW(src.c_str(), dst.c_str())) {
        wcout << L"Файл перемещён/переименован в: " << dst << endl;
    } else {
        wcerr << L"Ошибка перемещения файла. Код: " << GetLastError() << endl;
    }
}

void FileAttributes() {
    wcin.ignore();
    wstring filePath;
    wcout << L"Введите путь к файлу/каталогу: ";
    getline(wcin, filePath);

    DWORD attrs = GetFileAttributesW(filePath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        wcerr << L"Ошибка чтения атрибутов. Код: " << GetLastError() << endl;
        return;
    }
    wcout << L"Текущие атрибуты (число): " << attrs << endl;

    wcout << L"\nХотите изменить скрытый атрибут?\n"
          << L"1 - Установить скрытым\n"
          << L"2 - Снять скрытый\n"
          << L"0 - Не менять\n"
          << L"Ваш выбор: ";
    int c;
    wcin >> c;
    if (c == 1) {
        attrs |= FILE_ATTRIBUTE_HIDDEN;
        if (!SetFileAttributesW(filePath.c_str(), attrs)) {
            wcerr << L"Ошибка установки скрытости. Код: " << GetLastError() << endl;
        } else {
            wcout << L"Скрытость установлена." << endl;
        }
    } else if (c == 2) {
        attrs &= ~FILE_ATTRIBUTE_HIDDEN;
        if (!SetFileAttributesW(filePath.c_str(), attrs)) {
            wcerr << L"Ошибка снятия скрытости. Код: " << GetLastError() << endl;
        } else {
            wcout << L"Скрытый атрибут снят." << endl;
        }
    }

    HANDLE hFile = CreateFileW(filePath.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        wcerr << L"Ошибка открытия файла для работы со временем. Код: " << GetLastError() << endl;
        return;
    }

    LARGE_INTEGER fsize;
    if (GetFileSizeEx(hFile, &fsize)) {
        wcout << L"\nРазмер файла: " << fsize.QuadPart << L" байт" << endl;
    }


    FILETIME ftCreate, ftWrite;

    if (!GetFileTime(hFile, &ftCreate, nullptr, &ftWrite)) {
        wcerr << L"Не удалось получить время файла. Код: " << GetLastError() << endl;
        CloseHandle(hFile);
        return;
    }

    {
        SYSTEMTIME stCreateUTC, stCreateLocal;
        FileTimeToSystemTime(&ftCreate, &stCreateUTC);
        SystemTimeToTzSpecificLocalTime(nullptr, &stCreateUTC, &stCreateLocal);
        wcout << L"\nВремя создания: "
              << stCreateLocal.wDay << L"/" << stCreateLocal.wMonth << L"/" << stCreateLocal.wYear
              << L" " << stCreateLocal.wHour << L":" << stCreateLocal.wMinute << L":" << stCreateLocal.wSecond << endl;
    }
    {
        SYSTEMTIME stWrUTC, stWrLocal;
        FileTimeToSystemTime(&ftWrite, &stWrUTC);
        SystemTimeToTzSpecificLocalTime(nullptr, &stWrUTC, &stWrLocal);
        wcout << L"Время последней записи: "
              << stWrLocal.wDay << L"/" << stWrLocal.wMonth << L"/" << stWrLocal.wYear
              << L" " << stWrLocal.wHour << L":" << stWrLocal.wMinute << L":" << stWrLocal.wSecond << endl;
    }

    wcout << L"\nИзменить время на текущее?\n"
          << L"1 - Время создания\n"
          << L"2 - Время последней записи\n"
          << L"3 - Установить оба\n"
          << L"0 - Не менять\n"
          << L"Ваш выбор: ";
    int timeCho;
    wcin >> timeCho;
    FILETIME *pC = nullptr;
    FILETIME *pW = nullptr;

    if (timeCho != 0) {
        SYSTEMTIME stUTCNow;
        GetSystemTime(&stUTCNow);
        FILETIME ftNow;
        SystemTimeToFileTime(&stUTCNow, &ftNow);

        if (timeCho == 1) {
            pC = &ftNow;
        } else if (timeCho == 2) {
            pW = &ftNow;
        } else if (timeCho == 3) {
            pC = &ftNow;
            pW = &ftNow;
        }

        if (!SetFileTime(hFile, pC, nullptr, pW)) {
            wcerr << L"Не удалось изменить время. Код: " << GetLastError() << endl;
        } else {
            wcout << L"Время файла успешно изменено." << endl;
        }
    }

    CloseHandle(hFile);
}

int main() {
    SetConsoleToUTF8();
    setlocale(LC_ALL, "ru_RU.UTF-8");

    while (true) {
        wcout << L"\n+----------------------------------+\n";
        wcout << L"|           ГЛАВНОЕ МЕНЮ           |\n";
        wcout << L"|----------------------------------|\n";
        wcout << L"| [1] Список дисков                |\n";
        wcout << L"| [2] Информация о диске           |\n";
        wcout << L"| [3] Создать каталог              |\n";
        wcout << L"| [4] Удалить каталог              |\n";
        wcout << L"| [5] Создать файл                 |\n";
        wcout << L"| [6] Копировать файл              |\n";
        wcout << L"| [7] Переместить файл             |\n";
        wcout << L"| [8] Атрибуты/время файла         |\n";
        wcout << L"| [0] Выйти из программы           |\n";
        wcout << L"+----------------------------------+\n";
        wcout << L"Ваш выбор: ";

        int choice;
        if (!(wcin >> choice)) {
            wcin.clear();
            wcin.ignore(numeric_limits<streamsize>::max(), L'\n');
            ClearScreen();
            wcout << L"Некорректный ввод. Попробуйте снова.\n";
            continue;
        }

        switch (choice) {
            case 0:
                wcout << L"Завершение работы.\n";
                return 0;
            case 1:
                ListDrives();
                break;
            case 2:
                ShowDriveInfo();
                break;
            case 3:
                CreateDirectory();
                break;
            case 4:
                RemoveDirectory();
                break;
            case 5:
                CreateFile();
                break;
            case 6:
                CopyFile();
                break;
            case 7:
                MoveFile();
                break;
            case 8:
                FileAttributes();
                break;
            default:
                ClearScreen();
                wcout << L"Нет такого пункта. Повторите ввод.\n";
                continue;
        }

        WaitForKeyPress();
        ClearScreen();
    }
    return 0;
}
