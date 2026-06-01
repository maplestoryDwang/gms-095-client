#include "pch.h"
#include "hook.h"
#include "config.h"
#include "debug.h"
#include <intrin.h>


typedef decltype(&SetUnhandledExceptionFilter) SetUnhandledExceptionFilter_t;
static SetUnhandledExceptionFilter_t SetUnhandledExceptionFilter_orig = reinterpret_cast<SetUnhandledExceptionFilter_t>(GetAddress("KERNEL32", "SetUnhandledExceptionFilter"));

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter_hook(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
    // ZExceptionHandler::ZExceptionHandler()
    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == 0x007433F9) {
        AttachStringPoolMod();  // hook stringpool
    }
    return SetUnhandledExceptionFilter_orig(lpTopLevelExceptionFilter);
}


typedef decltype(&CreateMutexA) CreateMutexA_t;
static CreateMutexA_t CreateMutexA_orig = reinterpret_cast<CreateMutexA_t>(GetAddress("KERNEL32", "CreateMutexA"));

HANDLE WINAPI CreateMutexA_hook(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) {
    DEBUG_MESSAGE("CreateMutexA : %s", lpName);
    if (lpName && !strcmp(lpName, "WvsClientMtx")) {
        char sMutex[128];
        sprintf_s(sMutex, 128, "%s-%d", lpName, GetCurrentProcessId());
        lpName = sMutex;
        AttachClientHooks();
        return CreateMutexA_orig(lpMutexAttributes, bInitialOwner, sMutex);
    }
    return CreateMutexA_orig(lpMutexAttributes, bInitialOwner, lpName);
}


typedef decltype(&CreateWindowExA) CreateWindowExA_t;
static CreateWindowExA_t CreateWindowExA_orig = reinterpret_cast<CreateWindowExA_t>(GetAddress("USER32", "CreateWindowExA"));
static WNDPROC g_WndProc;

LRESULT WndProc_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    static POINT ptOffset;
    static bool bMoving;
    switch (Msg) {
        // 自由拖动
    case WM_NCMOUSEMOVE:
    case WM_MOUSEMOVE:
        if (bMoving) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                SetWindowPos(hWnd, NULL, ptCursor.x - ptOffset.x, ptCursor.y - ptOffset.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            } else {
                bMoving = false;
                ReleaseCapture();
            }
        }
        break;
    case WM_NCLBUTTONDOWN:
        if (wParam == HTMENU || wParam == HTLEFT) {
            break;
        } else if (wParam == HTCAPTION) {
            RECT rcWnd;
            POINT ptCursor;
            GetWindowRect(hWnd, &rcWnd);
            GetCursorPos(&ptCursor);
            ptOffset.x = ptCursor.x - rcWnd.left;
            ptOffset.y = ptCursor.y - rcWnd.top;
            SetCapture(hWnd);
            bMoving = true;
        }
        return 0;
    case WM_NCLBUTTONUP:
    case WM_LBUTTONUP:
        if (wParam == HTCLOSE) {
            PostQuitMessage(0);
        } else if (wParam == HTMINBUTTON && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            ShowWindow(hWnd, SW_MINIMIZE);
        }
        bMoving = false;
        ReleaseCapture();
        break;
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
        return 0;
    case WM_RBUTTONUP:
        if (!bMoving) {
            break;
        }
        return 0;
    }
    return CallWindowProcA(g_WndProc, hWnd, Msg, wParam, lParam);
}

HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExA_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (lpClassName && !strcmp(lpClassName, "MapleStoryClass")) {
        SetWindowText(hWnd, CONFIG_WINDOW_NAME);

        // WndProc_hook 这是一个标准的 Windows 窗口消息处理器（窗口钩子），主要针对鼠标点击、拖动进行特殊魔改：
        g_WndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc_hook)));
    }
    return hWnd;
}


typedef decltype(&RegCreateKeyExA) RegCreateKeyExA_t;
static RegCreateKeyExA_t RegCreateKeyExA_orig = reinterpret_cast<RegCreateKeyExA_t>(GetAddress("ADVAPI32", "RegCreateKeyExA"));

LSTATUS WINAPI RegCreateKeyExA_hook(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) {
   
    // 原版游戏可能会尝试在系统的 HKEY_LOCAL_MACHINE（根主键，需要管理员权限）下创建或读取游戏的注册表项（例如保存分辨率、声音设置）。
    // 这个 Hook 把所有的注册表创建请求，强行改为了 HKEY_CURRENT_USER（当前用户主键，不需要管理员权限）。
    return RegCreateKeyExA_orig(HKEY_CURRENT_USER, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}


typedef decltype(&WSPStartup) WSPStartup_t;
static WSPStartup_t WSPStartup_orig = reinterpret_cast<WSPStartup_t>(GetAddress("MSWSOCK", "WSPStartup"));
static WSPPROC_TABLE g_ProcTable;
static ULONG g_uNexonAddress;


// 当游戏尝试连接网络时触发。它会用 InetNtopA 把网络地址转换成字符串（如 "211.x.x.x"）。
// 具体改变：它检查连接的 IP 里是否包含官方的特征（CONFIG_NEXON_SEARCH）。如果包含，它会把官方 IP 擦掉，强行写入你自己的私服 IP（CONFIG_SERVER_ADDRESS）和端口（g_nServerPort）。
//        同时，它用 g_uNexonAddress 变量悄悄记录下原本官方的 IP 地址。 
int WINAPI WSPConnect_hook(SOCKET s, const struct sockaddr FAR* name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno) {
    char sName[INET_ADDRSTRLEN];
    InetNtopA(AF_INET, &((sockaddr_in*)name)->sin_addr, sName, INET_ADDRSTRLEN);
    if (strstr(sName, CONFIG_NEXON_SEARCH)) {
        g_uNexonAddress = ((sockaddr_in*)name)->sin_addr.S_un.S_addr;
        InetPtonA(AF_INET, g_sServerAddress ? g_sServerAddress : CONFIG_SERVER_ADDRESS, &((sockaddr_in*)name)->sin_addr.S_un.S_addr);
        if (g_nServerPort) {
            ((sockaddr_in*)name)->sin_port = htons(static_cast<u_short>(g_nServerPort));
        }
    }
    return g_ProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}


// 作用（反检测欺骗）：这是为了对付游戏客户端的“反反向工程检测”。
// 游戏连上服务器后，内部代码可能会不放心，会再次调用 WSPGetPeerName 询问系统：“我现在连着的这个服务器 IP 到底是多少？” 如果发现返回的不是官方 IP，游戏就会报网络错误。
// 具体改变：这个 Hook 在游戏询问时，把刚才记录的官方旧 IP（g_uNexonAddress）伪造并填回给游戏。 
int WINAPI WSPGetPeerName_hook(SOCKET s, struct sockaddr* name, LPINT namelen, LPINT lpErrNo) {
    int result = g_ProcTable.lpWSPGetPeerName(s, name, namelen, lpErrNo);
    ((sockaddr_in*)name)->sin_addr.S_un.S_addr = g_uNexonAddress;
    return result;
}


// 原理：该版本游戏没有使用普通的 connect 函数，而是使用了 Windows 的服务提供者接口（SPI，即 WSP 层的底层网络过滤）。
// 拦截网络底层的启动初始化。游戏在加载网络库时，Hook 强行把底层的“连接函数指针（lpWSPConnect）”和“获取对方名称函数指针（lpWSPGetPeerName）”替换成了我们自定义的 Hook 封包。
int WINAPI WSPStartup_hook(WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable) {
    int result = WSPStartup_orig(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);
    g_ProcTable = *lpProcTable;
    lpProcTable->lpWSPConnect = &WSPConnect_hook;
    lpProcTable->lpWSPGetPeerName = &WSPGetPeerName_hook;
    return result;
}


typedef decltype(&FileTimeToSystemTime) FileTimeToSystemTime_t;
static FileTimeToSystemTime_t FileTimeToSystemTime_orig = reinterpret_cast<FileTimeToSystemTime_t>(GetAddress("KERNEL32", "FileTimeToSystemTime"));

// 作用：本地时区时间修复。
// 它把标准 UTC 时间自动转换为了当前运行游戏这台电脑的本地时区时间（例如北京时间 GMT+8），确保游戏内的时钟、商城道具过期倒计时显示的都是正确的本地时间。
BOOL WINAPI FileTimeToSystemTime_hook(const FILETIME* lpFileTime, LPSYSTEMTIME lpSystemTime) {
    if (!FileTimeToSystemTime_orig(lpFileTime, lpSystemTime)) {
        return FALSE;
    }
    return SystemTimeToTzSpecificLocalTime(nullptr, lpSystemTime, lpSystemTime);
}


void AttachSystemHooks() {
    // 作用：拦截 Windows 的全局未捕获异常过滤器设置。
    // hook 字符串
    ATTACH_HOOK(SetUnhandledExceptionFilter_orig, SetUnhandledExceptionFilter_hook);


    // 作用：实现客户端多开（解除单开限制）。
    ATTACH_HOOK(CreateMutexA_orig, CreateMutexA_hook);

    // 作用：接管游戏窗口创建、自定义窗口标题、实现窗口拖动/防卡死。
    ATTACH_HOOK(CreateWindowExA_orig, CreateWindowExA_hook);

    // 作用：重定向注册表权限，防止因权限不足导致游戏打不开。
    ATTACH_HOOK(RegCreateKeyExA_orig, RegCreateKeyExA_hook);

    // 作用：核心网络劫持。把官方的服务器 IP 和端口，强行变成你自己的私服 IP。
    ATTACH_HOOK(WSPStartup_orig, WSPStartup_hook);

    // 作用：本地时区时间修复。
    ATTACH_HOOK(FileTimeToSystemTime_orig, FileTimeToSystemTime_hook);
}