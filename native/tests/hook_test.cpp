#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

// 测试进程枚举
void TestProcessEnum() {
    std::cout << "\n=== Testing Process Enumeration ===" << std::endl;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create snapshot" << std::endl;
        return;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);

    if (Process32FirstW(hSnapshot, &pe)) {
        int count = 0;
        do {
            // 检查是否能看到我们的进程
            std::wstring name(pe.szExeFile);
            if (name.find(L"pqy") != std::wstring::npos) {
                std::cout << "FOUND our process: ";
                std::wcout << name << std::endl;
            }
            count++;
        } while (Process32NextW(hSnapshot, &pe));

        std::cout << "Total processes visible: " << count << std::endl;
    }

    CloseHandle(hSnapshot);
}

// 测试调试器检测
void TestDebuggerDetection() {
    std::cout << "\n=== Testing Debugger Detection ===" << std::endl;

    BOOL isDebuggerPresent = IsDebuggerPresent();
    std::cout << "IsDebuggerPresent: " << (isDebuggerPresent ? "TRUE" : "FALSE") << std::endl;

    BOOL isRemoteDebugger = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemoteDebugger);
    std::cout << "CheckRemoteDebuggerPresent: " << (isRemoteDebugger ? "TRUE" : "FALSE") << std::endl;
}

// 主函数
int main() {
    std::cout << "PQY Hook Test Program" << std::endl;
    std::cout << "=====================" << std::endl;

    // 加载 Hook DLL
    HMODULE hHookDll = LoadLibraryA("pqy_hook.dll");
    if (!hHookDll) {
        std::cout << "Failed to load pqy_hook.dll" << std::endl;
        return 1;
    }

    std::cout << "pqy_hook.dll loaded successfully" << std::endl;

    // 获取导出函数
    typedef bool(__cdecl *InstallHooksFunc)();
    typedef int(__cdecl *GetHookCountFunc)();

    auto InstallHooks = (InstallHooksFunc)GetProcAddress(hHookDll, "PQY_InstallHooks");
    auto GetHookCount = (GetHookCountFunc)GetProcAddress(hHookDll, "PQY_GetHookCount");

    if (InstallHooks) {
        InstallHooks();
        std::cout << "Hooks installed" << std::endl;
    }

    if (GetHookCount) {
        std::cout << "Hook count: " << GetHookCount() << std::endl;
    }

    // 测试
    TestDebuggerDetection();
    TestProcessEnum();

    // 等待用户输入
    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();

    // 卸载
    FreeLibrary(hHookDll);

    return 0;
}
