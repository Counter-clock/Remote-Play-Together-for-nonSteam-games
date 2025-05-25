#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <Windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "version.lib")

struct GamePath 
{ 
    std::string directory, filename, displayname; 
};

int main()
{
    const char* configFile = "remoteplay.txt";
    const char* header = "Remote Play Together";

    const size_t headerSizeDefault = strlen(header) + 9;
    size_t headerSize = headerSizeDefault;

    std::vector<GamePath> games;
    games.reserve(10);

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTextAttribute(console, 15);

    DWORD mode; GetConsoleMode(console, &mode); 
    bool isVTP = SetConsoleMode(console, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleCtrlHandler([](DWORD a) -> BOOL { HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); wchar_t pB[MAX_PATH]; if (ExpandEnvironmentStringsW(L"%LOCALAPPDATA%", pB, MAX_PATH)) MoveFileExW((std::wstring(pB) + L"\\Packages\\Microsoft.WindowsTerminal_8wekyb3d8bbwe\\LocalState\\originalsettings.txt").c_str(), (std::wstring(pB) + L"\\Packages\\Microsoft.WindowsTerminal_8wekyb3d8bbwe\\LocalState\\settings.json").c_str(), MOVEFILE_REPLACE_EXISTING); SetConsoleTextAttribute(h, 7); CONSOLE_CURSOR_INFO c = { 100, true }; SetConsoleCursorInfo(h, &c); CONSOLE_SCREEN_BUFFER_INFO s; GetConsoleScreenBufferInfo(h, &s); DWORD n; FillConsoleOutputCharacterW(h, ' ', s.dwSize.X * s.dwSize.Y, { 0, 0 }, &n); SetConsoleCursorPosition(h, { 0, 0 }); ExitProcess(0); return TRUE; }, TRUE);
        
    std::wstring wRelativePath, wConhostPath, wSettingsPath, wOriginalSettingsPath;
    {
        wchar_t pathBuffer[MAX_PATH];
        if (ExpandEnvironmentStringsW(L"%LOCALAPPDATA%", pathBuffer, MAX_PATH))
        {
            wSettingsPath = std::wstring(pathBuffer) + L"\\Packages\\Microsoft.WindowsTerminal_8wekyb3d8bbwe\\LocalState\\settings.json";
            wOriginalSettingsPath = std::wstring(pathBuffer) + L"\\Packages\\Microsoft.WindowsTerminal_8wekyb3d8bbwe\\LocalState\\originalsettings.txt";
        }

        GetModuleFileNameW(NULL, pathBuffer, MAX_PATH);
        wRelativePath = std::wstring(pathBuffer).substr(0, std::wstring(pathBuffer).find_last_of(L"\\/")) + L"\\";

        (void)GetWindowsDirectoryW(pathBuffer, MAX_PATH);
        wConhostPath = std::wstring(pathBuffer) + L"\\System32\\conhost.exe";
    }
    
    auto convertToWString = [](const std::string& str) -> std::wstring
        {
            int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
            if (size > 0) {
                std::wstring wStr(size - 1, 0);
                MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, &wStr[0], size);
                return wStr;
            }

            size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
            if (size > 0) {
                std::wstring wStr(size - 1, 0);
                MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wStr[0], size);
                return wStr;
            }

            return L"";
        };

    auto convertToString = [](const std::wstring& wStr) -> std::string
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string str(size - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, &str[0], size - 1, nullptr, nullptr);
            return str;
        };
    std::string relativePath = convertToString(wRelativePath);

    const char* info_main, * info, * setup, * paper, * playbutton, * r_arrow, * lock, * warning, * watch, * lightbulb, * leave; int dif_info = 0, dif_warn = 0;
    {
        DWORD dwSize = GetFileVersionInfoSizeW(wConhostPath.c_str(), nullptr); int WindowsVersion = 0;
        if (dwSize != 0)
        {
            std::vector<BYTE> versionData(dwSize); GetFileVersionInfoW(wConhostPath.c_str(), 0, dwSize, versionData.data());
            VS_FIXEDFILEINFO* fileInfo = nullptr; VerQueryValueW(versionData.data(), L"\\", reinterpret_cast<void**>(&fileInfo), nullptr);

            if (fileInfo != nullptr) WindowsVersion = HIWORD(fileInfo->dwFileVersionLS);
        }
        wchar_t windowClass[30]; GetClassNameW(GetForegroundWindow(), windowClass, 30);

        bool isWindowsTerminal = std::wstring(windowClass) == L"CASCADIA_HOSTING_WINDOW_CLASS";
        bool isWindows11 = WindowsVersion >= 22000;

        if (isWindows11 || isWindowsTerminal)
        {
            info_main = reinterpret_cast<const char*>(u8" \u2139  ");
            setup = reinterpret_cast<const char*>(u8" \U0001F6E0  ");
            paper = reinterpret_cast<const char*>(u8" \U0001F5D2  ");
            playbutton = reinterpret_cast<const char*>(u8" \u25B6\uFE0F");
            r_arrow = reinterpret_cast<const char*>(u8"\u2192");

            lock = reinterpret_cast<const char*>(u8"\U0001F513");
            info = reinterpret_cast<const char*>(u8" \u2139  ");
            warning = reinterpret_cast<const char*>(u8" \u26A0  ");
            watch = reinterpret_cast<const char*>(u8" \u23F3 ");
            lightbulb = reinterpret_cast<const char*>(u8" \U0001F4A1 ");
            leave = reinterpret_cast<const char*>(u8"\u21A9\uFE0F");
        }
        else 
        {
            info_main = " () "; 
            setup = " () ";
            paper = " () ";
            playbutton = " () ";
            r_arrow = ""; 

            lock = " $";
            info = " i "; dif_info = -2;
            warning = " X "; dif_warn = 1;
            watch = " > ";
            lightbulb = " () ";
            leave = "";
        }

        if (isWindowsTerminal && MoveFileW(wSettingsPath.c_str(), wOriginalSettingsPath.c_str()))
        {
            HANDLE from = INVALID_HANDLE_VALUE;
            for (int i = 0; i < 100; i++)
            {
                Sleep(10);

                from = CreateFileW(wSettingsPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (from != INVALID_HANDLE_VALUE) break;
            }

            std::vector<std::string> lines; lines.reserve(75); std::vector<char> charBuffer; LARGE_INTEGER filesize; DWORD bytesRead;
            if (from != INVALID_HANDLE_VALUE && GetFileSizeEx(from, &filesize) && (charBuffer.resize(static_cast<size_t>(filesize.QuadPart)), ReadFile(from, charBuffer.data(), static_cast<DWORD>(filesize.QuadPart), &bytesRead, nullptr)))
            {
                size_t nullCount = 0;
                for (size_t i = 0; i < charBuffer.size(); ++i) nullCount += (charBuffer[i] == '\0');

                if (nullCount > charBuffer.size() / 4) {
                    std::wstring FileUtf16(reinterpret_cast<const wchar_t*>(charBuffer.data()), charBuffer.size() / sizeof(wchar_t));
                    std::string FileUtf8 = convertToString(FileUtf16);
                    charBuffer.assign(FileUtf8.begin(), FileUtf8.end());
                }

                std::string line; size_t lineSplit = 0, lineCount = 0; auto start = charBuffer.begin();
                for (int i = 0; i < charBuffer.size(); i++)
                {
                    if (charBuffer[i] == '\n')
                    {
                        line = std::string(start + lineSplit, start + i - (i > 0 && charBuffer[i - 1] == '\r' ? 1 : 0));
                        while (i < charBuffer.size() - 2 && (charBuffer[i] == '\n' || charBuffer[i] == '\r')) i++;
                        lineSplit = i;
                    }
                    else if (i == charBuffer.size() - 1) line = std::string(start + lineSplit, start + i + 1);
                    else continue;

                    if (line.find(R"("defaults": {},)") != std::string::npos)
                        line = R"x(        "defaults": { "adjustIndistinguishableColors": "never", "autoMarkPrompts": false, "backgroundImage": null, "colorScheme": "SCHEME", "experimental.retroTerminalEffect": true, "experimental.rightClickContextMenu": true, "font": { "colorGlyphs": true, "weight": "medium" }, "intenseTextStyle": "bright", "opacity": 40, "padding": "8", "showMarksOnScrollbar": false, "tabTitle": "\ud83d\udcf6  |  Remote Play Together", "useAcrylic": true },)x";
                    else if (line.find(R"("schemes": [],)") != std::string::npos)
                        line = R"x(    "schemes": [ { "background": "#03162B", "black": "#0C0C0C", "blue": "#0037DA", "brightBlack": "#767676", "brightBlue": "#3B78FF", "brightCyan": "#61D6D6", "brightGreen": "#16C60C", "brightPurple": "#B4009E", "brightRed": "#E74856", "brightWhite": "#FFFFFF", "brightYellow": "#F9F1A5", "cursorColor": "#FFFFFF", "cyan": "#3A96DD", "foreground": "#CCCCCC", "green": "#13A10E", "name": "SCHEME", "purple": "#881798", "red": "#C50F1F", "selectionBackground": "#BFBFBF", "white": "#CCCCCC", "yellow": "#C19C00" } ], "showTabsInTitlebar": true, "showTerminalTitleInTitlebar": false, "theme": "legacyLight", "tabWidthMode": "titleLength", "useAcrylicInTabRow": true,)x";

                    lines.push_back(line);
                }
            }
            if (from != INVALID_HANDLE_VALUE) CloseHandle(from);

            if (!lines.empty())
            {
                HANDLE to = CreateFileW(wSettingsPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (to != INVALID_HANDLE_VALUE)
                {
                    for (const std::string& text : lines) WriteFile(to, (text + "\n").c_str(), static_cast<DWORD>((text + "\n").size()), nullptr, nullptr);
                    SetForegroundWindow(GetConsoleWindow()); Sleep(100); WriteFile(to, "\n", 1, nullptr, nullptr);
                    CloseHandle(to);
                }
            }

        }
    }

    auto clearConsole = [&console]
        {
            CONSOLE_SCREEN_BUFFER_INFO screenInfo;
            GetConsoleScreenBufferInfo(console, &screenInfo);
            DWORD n; FillConsoleOutputCharacterW(console, ' ', screenInfo.dwSize.X * screenInfo.dwSize.Y, { 0, 0 }, &n);
            SetConsoleCursorPosition(console, { 0, 0 });
        };

    auto cursor = [&console](bool show)
        {
            CONSOLE_CURSOR_INFO cursor = { 100, show };
            SetConsoleCursorInfo(console, &cursor);
        };
    cursor(false);

    auto cleanUp = [&]()
        {
            MoveFileExW(wOriginalSettingsPath.c_str(), wSettingsPath.c_str(), MOVEFILE_REPLACE_EXISTING);
            SetConsoleTextAttribute(console, 7); 
            cursor(true);
            clearConsole();
        };

    auto waitForKey = [&]()
        {
            while (_kbhit()) (void)_getch();
            if (_getch() == 3) { cleanUp(); ExitProcess(0); }
        };

    auto loadFile = [&]()
        {
            games.clear();
            std::wstring path = wRelativePath + convertToWString(configFile); 

            HANDLE read = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr); std::vector<char> charBuffer; LARGE_INTEGER filesize; DWORD bytesRead;
            if (read == INVALID_HANDLE_VALUE || !GetFileSizeEx(read, &filesize) || (charBuffer.resize(static_cast<size_t>(filesize.QuadPart)), !ReadFile(read, charBuffer.data(), static_cast<DWORD>(filesize.QuadPart), &bytesRead, nullptr)))
            {
                if (read != INVALID_HANDLE_VALUE) CloseHandle(read);
                HANDLE write = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (write == INVALID_HANDLE_VALUE)
                {
                    clearConsole();
                    std::cout << std::string(11, '-') << std::endl;
                    std::cout << setup << "Setup" << std::endl;
                    std::cout << std::string(11, '-') << std::endl;
                    std::cout << "Unable to create: " << relativePath + configFile << "\n" << std::endl;
                    std::cout << std::string(64, '-') << std::endl;
                    std::cout << lightbulb << "Try running as administrator or creating the file manually" << std::endl;
                    std::cout << std::string(64, '-') << std::endl;
                    waitForKey();
                    cleanUp();
                    exit(1);
                }
                WriteFile(write, u8"(Add all your game paths here)", 30, nullptr, nullptr);
                CloseHandle(write);
            }
            else 
            {
                size_t nullCount = 0;
                for (size_t i = 0; i < charBuffer.size(); ++i) nullCount += (charBuffer[i] == '\0');

                if (nullCount > charBuffer.size() / 4) {
                    std::wstring FileUtf16(reinterpret_cast<const wchar_t*>(charBuffer.data()), charBuffer.size() / sizeof(wchar_t));
                    std::string FileUtf8 = convertToString(FileUtf16);
                    charBuffer.assign(FileUtf8.begin(), FileUtf8.end());
                }

                std::string line; size_t lineSplit = 0; auto start = charBuffer.begin();
                for (int i = 0; i < charBuffer.size(); i++)
                {
                    if (charBuffer[i] == '\n')
                    {
                        line = std::string(start + lineSplit, start + i - (i > 0 && charBuffer[i - 1] == '\r' ? 1 : 0));
                        while (i < charBuffer.size() - 2 && (charBuffer[i] == '\n' || charBuffer[i] == '\r')) i++;
                        lineSplit = i;
                    }
                    else if (i == charBuffer.size() - 1) line = std::string(start + lineSplit, start + i + 1);
                    else continue;

                    if (line.find_first_of(":") != std::string::npos && line.find_first_of(":") > 0 && line[line.find_first_of(":") - 1] != ' ')
                    {
                        line.erase(0, line.find_first_of(":") - 1);

                        size_t displaynameSplitOffset = line.find_first_of("|") != std::string::npos ? line.substr(line.find_first_of("|") + 1).find_first_not_of(" ") : std::string::npos;
                        size_t displaynameIndex = displaynameSplitOffset != std::string::npos ? line.find_first_of("|") + 1 + displaynameSplitOffset : std::string::npos;

                        std::string lowercaseExe = line.substr(0, displaynameIndex);
                        for (char& c : lowercaseExe)
                            if (c >= 'A' && c <= 'Z') c += 32;

                        size_t exeIndex = lowercaseExe.substr(0, displaynameIndex).find(".exe");
                        size_t filenameIndex = line.substr(0, exeIndex).find_last_of("\\/") != std::string::npos ? line.substr(0, exeIndex).find_last_of("\\/") + 1 : std::string::npos;

                        if (exeIndex != std::string::npos && filenameIndex != std::string::npos)
                        {
                            GamePath object;
                            object.directory = line.substr(0, filenameIndex);
                            object.filename = line.substr(filenameIndex, exeIndex + 4 - filenameIndex);

                            if (displaynameIndex != std::string::npos)
                                object.displayname = line.substr(displaynameIndex, line.find_last_not_of(" ") + 1 - displaynameIndex);
                            else
                                object.displayname = line.substr(filenameIndex, exeIndex - filenameIndex);

                            for (char& c : object.directory)
                                if (c == '/') c = '\\';

                            object.displayname = convertToString(convertToWString(object.displayname));

                            games.push_back(object);
                        }
                    }
                }
                CloseHandle(read);
            }
            
            size_t maxStringLength = 0, maxStringIndex = 0;
            for (size_t i = 0; i < games.size(); ++i) {
                size_t newLength = convertToWString(games[i].displayname).length();
                if (newLength > maxStringLength)
                {
                    maxStringLength = newLength;
                    maxStringIndex = i + 1;
                }
            }

            size_t newWidth = std::to_string(maxStringIndex).length() + maxStringLength + 5 + 1;
            if (newWidth > headerSizeDefault)
                headerSize = newWidth;
            else headerSize = headerSizeDefault;
        };

    auto printHeader = [&]()
        {
            loadFile();
            clearConsole();
            std::cout << std::string(headerSize, '-') << std::endl;
            std::cout << "|" << std::string(((headerSize - strlen(header) - 7) / 2) - dif_warn, ' ') << lock << " | " << header << std::string(((headerSize - strlen(header) - 7) / 2) + ((headerSize - strlen(header) - 7) % 2 != 0) + dif_warn, ' ') << "|" << std::endl;
            std::cout << std::string(headerSize, '-') << std::endl;
        };
    printHeader();

    int choice = 0;

    while (true)
    {
        if (games.empty())
        {
            clearConsole();
            std::cout << std::string(10, '-') << std::endl;
            std::cout << info_main << "Info" << std::endl;
            std::cout << std::string(10, '-') << std::endl;
            std::cout << "This program needs to be replacing the .exe file of a Remote Play-supported Steam game.\n"
                      << "Then it must be started through the Steam library as that game.\n" << std::endl;
            std::cout << std::string(11, '-') << std::endl;
            std::cout << setup << "Setup" << std::endl;
            std::cout << std::string(11, '-') << std::endl;
            std::cout << "Add all your game .exe file paths into " << configFile << ". One path per line.\n"
                      << "The file is located in the program's directory: " << relativePath + configFile << "\n" << std::endl;
            std::cout << std::string(18, '-') << std::endl;
            std::cout << paper << "File Example " << std::endl;
            std::cout << std::string(18, '-') << std::endl;
            std::cout << "D:\\Games\\VideoGame1\\videogame1.exe" << std::endl;
            std::cout << "D:\\Games\\VideoGame2\\videogame2.exe" << std::endl;
            std::cout << "C:\\SteamLibrary\\steamapps\\common\\VideoGame3\\videogame3.exe | Video Game 3" << std::endl;
            std::cout << "C:\\SteamLibrary\\steamapps\\common\\VideoGame4\\videogame4.exe | Game 4" << std::endl;
            std::cout << "C:\\EpicGames\\VideoGame5\\videogame5.exe | Custom Name\n" << std::endl;
            std::cout << "TIP: By adding \"|\" at the end of a line, you can set a custom name that will be displayed in the menu.\n" << std::endl;
            std::cout << std::string(66, '-') << std::endl;
            std::cout << playbutton << "Once you've added your game paths, press any key to continue " << r_arrow << std::endl;
            std::cout << std::string(66, '-') << std::endl;
            waitForKey();
            printHeader();
            continue;
        }

        if (games.size() != 1)
        {
            for (size_t i = 0; i < games.size(); i++)
                std::cout << "| " << i + 1 << " | " << games[i].displayname << std::endl;

            std::cout << std::string(headerSize, '-') << std::endl;
            std::cout << "| " << games.size() + 1 << " | Exit " << leave << std::endl;
            std::cout << std::string(headerSize, '-') << std::endl;

            std::cout << "Enter your choice: ";
            std::string input;
            try
            {
                cursor(true);
                std::getline(std::cin, input);
                cursor(false);
                choice = stoi(input);
            }
            catch (...)
            {
                printHeader();
                std::cout << "|" << warning << "| Enter a number" << std::string(headerSize - 22 + dif_warn, ' ') << "|" << std::endl;
                std::cout << std::string(headerSize, '-') << std::endl;
                continue;
            }

            if (choice < 1 || choice > games.size() + 1)
            {
                printHeader();
                std::string line1 = "|" + std::string(warning) + "| You entered: " + std::to_string(choice).substr(0, 7);
                std::string line2 = "|" + std::string(info) + "| Options: 1-" + std::to_string(games.size() + 1).substr(0, 9);
                std::cout << line1 << std::string(headerSize - (line1.length() - 1 - dif_info), ' ') << "|" << "\n"
                          << line2 << std::string(headerSize - (line2.length() - 1 - dif_info), ' ') << "|" << std::endl;
                std::cout << std::string(headerSize, '-') << std::endl;
                continue;
            }

            if (choice == games.size() + 1)
            {
                cleanUp();
                return 0;
            }
        }
        else
        {
            choice = 1;
            printHeader();
            for (int i = 0; i < 3; i++)
            {
                if (i != 0 && isVTP) std::cout << "\033[2A\r";
                if (i != 0 && !isVTP) printHeader();
                std::cout << "|" << watch << "| Opening" << std::string(i + 1, '.') << std::string(headerSize - 15 - (i + 1) + dif_warn, ' ') << "|" << std::endl;
                std::cout << std::string(headerSize, '-') << std::endl;
                Sleep(165);
            }
        }

        choice--;
        HINSTANCE result = ShellExecuteW(nullptr, L"open", convertToWString(games[choice].filename).c_str(), nullptr, convertToWString(games[choice].directory).c_str(), SW_SHOWDEFAULT);

        size_t displaynameLength = convertToWString(games[choice].displayname).length();
        if ((intptr_t)result <= 32)
        {
            printHeader();
            if (games.size() == 1) {
                std::cout << " " << games[choice].displayname << std::endl;
                std::cout << std::string(headerSize, '-') << std::endl;
            }
            if ((intptr_t)result == 5)
                 std::cout << "|" << warning << "| Access denied" << std::string(headerSize - 21 + dif_warn, ' ') << "|" << std::endl;
            else std::cout << "|" << warning << "| File not found" << std::string(headerSize - 22 + dif_warn, ' ') << "|" << std::endl;
                 std::cout << std::string(headerSize, '-') << std::endl;

            if (games.size() == 1) {
                waitForKey();
                printHeader();
            }
            continue;
        }

        headerSize = headerSizeDefault - 2;
        size_t length = displaynameLength + 3 > headerSize ? displaynameLength + 3 : headerSize;

        auto launchingHeader = [&clearConsole, &length]()
            {
                clearConsole();
                std::cout << std::string(length + 2, '-') << "\n|"
                          << std::string((length - 9) / 2, ' ') << "Launching"
                          << std::string((length - 9) / 2 + ((length - 9) % 2 != 0), ' ') << "|\n"
                          << std::string(length + 2, '-') << std::endl;
            };
        launchingHeader();
        
        int remainder = static_cast<int>(length % headerSize);
        for (size_t i = 1, j = 0, progress = 0; i <= headerSize; i++, j++)
        {
            progress += length / headerSize + (remainder-- > 0 ? 1 : 0);
            size_t dots = (j > headerSize / 6) ? ((j > 2 * headerSize / 6) ? 3 : 2) : 1;
            if (j >= (headerSize / 2)) j = 0;
            if (i == headerSize) { progress = length; dots = 3; }

            if (i != 1 && isVTP) std::cout << "\033[3A\r";
            else if (i != 1 && !isVTP) launchingHeader();
            std::cout << " " << games[choice].displayname << std::string(dots, '.') << std::string(3 - dots, ' ') << "\n"
                      << std::string(length + 2, '-') << "\n"
                      << "[" << std::string(progress, '#') << std::string(length - progress, ' ') << "]\n"
                      << std::string(length + 2, '-') << std::flush;
            Sleep(50);
        }

        Sleep(800);

        cleanUp();
        return 0;
    }
}