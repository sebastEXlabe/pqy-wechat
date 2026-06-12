#include <windows.h>
#include <stdio.h>

// 简单的日志函数
void SimpleLog(const char* msg) {
    // 输出到调试器
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");

    // 写入文件
    FILE* f = fopen("logs/pqy_simple.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

// DLL 入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            SimpleLog("[PQY] DLL_PROCESS_ATTACH");
            DisableThreadLibraryCalls(hModule);
            SimpleLog("[PQY] DLL loaded successfully");
            break;
        }

        case DLL_PROCESS_DETACH: {
            SimpleLog("[PQY] DLL_PROCESS_DETACH");
            break;
        }

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

// 导出函数
extern "C" __declspec(dllexport)
int __cdecl PQY_GetVersion() {
    return 1;
}
