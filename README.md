 # 目的
cpp和hook记录学习，总有人觉得自己了不起，只有自己变强才不会被人冷嘲热讽。

1. 了解插件的主要内容
2. 了解自己的单机需要做哪些hook
3. 新增

## 主要内容
- launcher.cpp 登陆器的类，负责启动Maplestory.exe 并通过detour注入一个dll。
- injector.cpp 注入器类，负责注入dll具体的实现
```
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL); // 优化性能，忽略线程创建通知
        ProcessCommandLine();                // 1. 解析命令行
        AttachSystemHooks();                 // 2. 挂钩（Hook）游戏函数
        break;

```

- system.cpp 负责所有的注入功能
```c++
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
```

- hook.h

申明游戏相关的内容AttachClientHooks，在CreateMutexA_hook中进行调用
```angular2html
inline void AttachClientHooks() {
    AttachClientBypass();                // 重构CWvsApp，负责构建
    AttachClientHelper();                // 显示各种ID
    AttachClientInlink();
    AttachSystemOptionMod();            // 系统配置多分辨率
    AttachTemporaryStatMod();
    AttachElementalDamageMod();
    AttachIconIconMod();                // 更新宠物在物品栏的icon
    //AttachQuickSlot();
    AttachExceptionHandler();
}

```
## Hook核心
1. system.cpp
   负责hook **MSWSOCK的WSPStartup**


```c
typedef decltype(&WSPStartup) WSPStartup_t;
static WSPStartup_t WSPStartup_orig = reinterpret_cast<WSPStartup_t>(GetAddress("MSWSOCK", "WSPStartup"));
static WSPPROC_TABLE g_ProcTable;
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


// 原理：该版本游戏没有使用普通的 connect 函数，而是使用了 Windows 的服务提供者接口（SPI，即 WSP 层的底层网络过滤）。
// 拦截网络底层的启动初始化。游戏在加载网络库时，Hook 强行把底层的“连接函数指针（lpWSPConnect）”和“获取对方名称函数指针（lpWSPGetPeerName）”替换成了我们自定义的 Hook 封包。
int WINAPI WSPStartup_hook(WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable) {
    int result = WSPStartup_orig(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);
    g_ProcTable = *lpProcTable;
    lpProcTable->lpWSPConnect = &WSPConnect_hook;
    lpProcTable->lpWSPGetPeerName = &WSPGetPeerName_hook;
    return result;
}


```

2. bypass.cpp
```c++
    PatchRetZero(0x004AB900); // DR_check  (反作弊核心主線程)
    PatchRetZero(0x0045EBD0); // Hidedll
    PatchRetZero(0x009BF6C0); // SendHSLog   (發送作弊日誌)
    PatchRetZero(0x009BF370); // CeTracer::Run  (內存追蹤保護)
```
相关联重构
```
CWvsApp__ctor_hook (构造函数重构)
CWvsApp__SetUp_hook (启动与加载重构)
CWvsApp__Run_hook & CallUpdate_hook (主循环重构)
```



## 新增功能
1. 新增分辨率
2. 新增长键盘





