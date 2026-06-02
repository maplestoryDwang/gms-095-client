#include "pch.h"
#include "hook.h"
#include "config.h"

extern "C" __declspec(dllexport) VOID DummyExport() {}

char* g_sServerAddress = nullptr;
long g_nServerPort = 0;

// 命令行中获取ip和端口号
void ProcessCommandLine() {
    char sAddress[16];
    long nPort;
    if (sscanf_s(GetCommandLineA(), "%s %d", sAddress, sizeof(sAddress), &nPort) == 2) {
        g_sServerAddress = reinterpret_cast<char*>(malloc(strlen(sAddress) + 1));
        strcpy_s(g_sServerAddress, strlen(sAddress) + 1, sAddress);
        g_nServerPort = nPort;
    }
}

// dll注入的入口
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL); // 优化性能，忽略线程创建通知
        ProcessCommandLine();                // 1. 解析命令行
        AttachSystemHooks();                 // 2. 挂钩（Hook）游戏函数
        break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}