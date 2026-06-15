#include "pch.h"
#include "hook.h"
#include "config.h"
#include "debug.h"
#include <intrin.h>


// decltype(...)：这是 C++ 11 引入的一个关键字，意思是 “推导表达式的类型”（Declare Type）。它只在编译期起作用，不会实际执行括号内的表达式。
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

        // 其他功能
        AttachClientHooks();

        // 设置锁名字，后面+了PID肯定不一样的
        return CreateMutexA_orig(lpMutexAttributes, bInitialOwner, sMutex);
    }
    return CreateMutexA_orig(lpMutexAttributes, bInitialOwner, lpName);
}


// 创建具有扩展窗口样式的重叠、弹出窗口或子窗口;否则，此函数与 CreateWindow 函数相同。 
// 有关创建窗口以及 CreateWindowEx的其他参数的完整说明的详细信息，请参阅 CreateWindow。
// https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-createwindowexw
typedef decltype(&CreateWindowExA) CreateWindowExA_t;
static CreateWindowExA_t CreateWindowExA_orig = reinterpret_cast<CreateWindowExA_t>(GetAddress("USER32", "CreateWindowExA"));

// WndProc_hook 会一直、频繁地反复执行（每当你移动鼠标、点击键盘时都在执行）。
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
        // 当你在标题栏按下左键（WM_NCLBUTTONDOWN 且参数为 HTCAPTION）时，代码记录了鼠标和窗口的相对坐标差（ptOffset），并开启拖动状态（bMoving = true）。
        // 在鼠标移动时（WM_MOUSEMOVE），代码用 SetWindowPos 实时、强制地刷新窗口位置。这实现了极为丝滑的、类似现代软件的窗口拖动体验。
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
        // 关闭：当你点击右上角的 X（HTCLOSE）时，代码直接发送 PostQuitMessage(0)，通知系统彻底、干净地退出游戏。
        if (wParam == HTCLOSE) {
            PostQuitMessage(0);

            // 最小化：当你点击最小化按钮（HTMINBUTTON）时，代码用 ShowWindow(hWnd, SW_MINIMIZE) 强行把游戏最小化到任务栏。
            // 但这里加了一个极聪明的设定：!(GetAsyncKeyState(VK_CONTROL) & 0x8000) —— 如果你按住 Ctrl 键再点最小化，
            // 这个最小化功能就会失效（这可能是为了防止玩家在激烈的游戏操作中，不小心误触 Ctrl + 鼠标点击导致游戏突然最小化而导致角色死亡）。
        } else if (wParam == HTMINBUTTON && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            ShowWindow(hWnd, SW_MINIMIZE);
        }
        bMoving = false;
        ReleaseCapture();
        break;
    // 代码拦截了 WM_NCRBUTTONDOWN 和 WM_NCRBUTTONUP（非客户区/标题栏的鼠标右键按下与释放），并直接 return 0;。
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
        return 0;
    case WM_RBUTTONUP:
        if (!bMoving) {
            break;
        }
        return 0;
    }
    // 这是最关键的。它的意思是：“如果传进来的鼠标消息不是上面那几种（比如是键盘输入、游戏画面渲染刷新等），
    // 那就原封不动地还给游戏原本的处理器 g_WndProc。”
    return CallWindowProcA(g_WndProc, hWnd, Msg, wParam, lParam);
}

HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExA_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (lpClassName && !strcmp(lpClassName, "MapleStoryClass")) {

        // 修改窗口的名字
        SetWindowText(hWnd, CONFIG_WINDOW_NAME);

        // 第二步（内层 Hook/子类化）： 拦截游戏窗口的所有鼠标和点击消息。原版游戏收到鼠标消息时，本来应该走游戏自带的逻辑，但现在必须先经过 WndProc_hook 的过滤。
        // WndProc_hook 这是一个标准的 Windows 窗口消息处理器（窗口钩子），主要针对鼠标点击、拖动进行特殊魔改：
        // 这一步是把 WndProc_hook 的地址登记给 Windows 系统。告诉系统：“以后只要这个窗口有任何风吹草动（鼠标、键盘消息），你别直接找游戏原版的处理函数，先来找我的 WndProc_hook！”
        // 窗口子类化（Window Subclassing）。

        // SetWindowLongPtr
        // SetWindowLongPtr 非常贴心，它在把新地址覆盖进去的一瞬间，会把被覆盖掉的“原版函数地址”作为返回值吐出来。
        // 参数 1：hWnd（目标是谁）指定你要修改哪一个窗口。因为一个电脑上可能有几十个窗口（微信、浏览器、游戏），必须传入句柄，明确告诉系统：“我要改的是《冒险岛》的这个窗口”。
        // 参数 2：GWLP_WNDPROC（修改哪个属性） 这是一个宏常量（Get / Set Window Long Pointer WNDPROC）。它是一个“偏移量”，告诉系统：“我要修改的是‘窗口消息处理函数指针’这一项”。
        // 窗口其实还有别的属性，比如传入 GWL_STYLE 可以修改窗口的边框样式。而这里传入 GWLP_WNDPROC，就是专门用来改控制权的。
        // 参数 3：reinterpret_cast < LONG_PTR > (&WndProc_hook)（改成什么值）就是你说的，把你写好的 WndProc_hook 函数的内存地址传进去，作为新的值。
        // 
        // 
        // 返回的是原来的地址
        g_WndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc_hook)));
    }
    return hWnd;
}


// https://learn.microsoft.com/zh-cn/windows/win32/api/winreg/nf-winreg-regcreatekeyexa
// &RegCreateKeyExA：获取 Windows API 函数 RegCreateKeyExA 的函数指针（地址）。
// decltype(...)：这是 C++ 11 引入的一个关键字，意思是 “推导表达式的类型”（Declare Type）。它只在编译期起作用，不会实际执行括号内的表达式。
// 如果不使用 decltype，程序员必须手动写出极其复杂的 Windows 函数指针类型定义，长成下面这样：
// typedef LSTATUS (WINAPI *RegCreateKeyExA_t)(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, const LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
typedef decltype(&RegCreateKeyExA) RegCreateKeyExA_t;
static RegCreateKeyExA_t RegCreateKeyExA_orig = reinterpret_cast<RegCreateKeyExA_t>(GetAddress("ADVAPI32", "RegCreateKeyExA"));

LSTATUS WINAPI RegCreateKeyExA_hook(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) {
   
    // 原版游戏可能会尝试在系统的 HKEY_LOCAL_MACHINE（根主键，需要管理员权限）下创建或读取游戏的注册表项（例如保存分辨率、声音设置）。
    // 这个 Hook 把所有的注册表创建请求，强行改为了 HKEY_CURRENT_USER（当前用户主键，不需要管理员权限）。
    return RegCreateKeyExA_orig(HKEY_CURRENT_USER, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}


typedef decltype(&WSPStartup) WSPStartup_t;
static WSPStartup_t WSPStartup_orig = reinterpret_cast<WSPStartup_t>(GetAddress("MSWSOCK", "WSPStartup"));
// 临时保存返回值
static WSPPROC_TABLE g_ProcTable;
// 保存旧的地址
static ULONG g_uNexonAddress;


// 当游戏尝试连接网络时触发。它会用 InetNtopA 把网络地址转换成字符串（如 "211.x.x.x"）。
// 具体改变：它检查连接的 IP 里是否包含官方的特征（CONFIG_NEXON_SEARCH）。如果包含，它会把官方 IP 擦掉，强行写入你自己的私服 IP（CONFIG_SERVER_ADDRESS）和端口（g_nServerPort）。
// 同时，它用 g_uNexonAddress 变量悄悄记录下原本官方的 IP 地址。 
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


// https://learn.microsoft.com/zh-cn/windows/win32/api/ws2spi/nf-ws2spi-wspstartup
// 原理：该版本游戏没有使用普通的 connect 函数，而是使用了 Windows 的服务提供者接口（SPI，即 WSP 层的底层网络过滤）。
// 拦截网络底层的启动初始化。游戏在加载网络库时，Hook 强行把底层的“连接函数指针（lpWSPConnect）”和“获取对方名称函数指针（lpWSPGetPeerName）”替换成了我们自定义的 Hook 封包。

/*

     WSPStartup

    [in] wVersionRequested
    调用方可以使用的最高版本的 Windows 套接字 SPI 支持。 高位字节指定次要版本 (修订版) 编号;低位字节指定主版本号。

    [out] lpWSPData
    指向 WSPDATA 数据结构的指针，该结构接收有关 Windows 套接字服务提供程序的信息。

    [in] lpProtocolInfo
    指向 WSAProtocol_Info 结构的指针，该结构定义所需协议的特征。 当单个提供程序 DLL 能够实例化多个不同的服务提供程序时，这尤其有用。

    [in] UpcallTable
    Winsock 2 DLL (Ws2_32.dll) 在 WSPUpCallTable 结构中传递的httpall调度表。


    [out] lpProcTable  这个是返回值
    指向 SPI 函数指针表的指针。 此表作为 WSPProc_Table 结构返回。

    LPWSPConnect 函数与对等机建立连接，交换连接数据，并根据提供的流规范指定所需的服务质量。
    LPWSPGetPeerName 函数获取套接字连接到的对等方的地址。


*/

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

// 这一层hook的全是dll里面的hook
void AttachSystemHooks() {
    // 作用：拦截 Windows 的全局未捕获异常过滤器设置。
    // hook 字符串
    ATTACH_HOOK(SetUnhandledExceptionFilter_orig, SetUnhandledExceptionFilter_hook);

    // 作用：实现客户端多开（解除单开限制）。
    // 注意！游戏相关的功能hook也在这里 
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