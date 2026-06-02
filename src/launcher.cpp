#include "config.h"
#include "debug.h"
#include <windows.h>
#include <detours.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    // #define CONFIG_DLL_NAME       "Kinoko.dll"
    if (!DetourCreateProcessWithDllExA("MapleStory.exe", lpCmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi, CONFIG_DLL_NAME, NULL)) {
        ErrorMessage("Could not start MapleStory.exe [%d]", GetLastError());
        return 1;
    }

    // ResumeThread：当 DLL 成功注入后，恢复游戏主线程。此时游戏开始运行，但在运行游戏自身的代码之前，系统会优先加载我们注入的 Kinoko.dll。
    // WaitForSingleObject：让注入器留在后台，一直等待游戏进程退出，最后获取退出码。
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD dwExitCode;
    if (!GetExitCodeProcess(pi.hProcess, &dwExitCode)) {
        ErrorMessage("GetExitCodeProcess failed [%d]", GetLastError());
        return 1;
    }
    return 0;
}