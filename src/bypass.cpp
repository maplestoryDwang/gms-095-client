#include "pch.h"
#include "hook.h"
#include "debug.h"
#include "ztl/ztl.h"
#include "wvs/util.h"
#include "wvs/wvsapp.h"
#include "wvs/wvscontext.h"
#include "wvs/inputsystem.h"
#include "wvs/actionman.h"
#include "wvs/config.h"
#include "wvs/stage.h"
#include "wvs/login.h"
#include "wvs/clientsocket.h"
#include "wvs/temporarystatview.h"
#include "wvs/ctrlwnd.h"

#include <windows.h>
#include <timeapi.h>

#pragma warning(disable : 4996)

ZALLOC_GLOBAL
ZALLOCEX(ZAllocAnonSelector, 0x00C6E67C)
ZALLOCEX(ZAllocStrSelector<char>, 0x00C6E6A8)
ZALLOCEX(ZAllocStrSelector<wchar_t>, 0x00C6E64C)
ZRECYCLABLE(ZInetAddr, 0x00C63EE4)


static auto CWvsApp__ctor = 0x009CA8A0;

// 接管遊戲主程序類 CWvsApp 的構造函數（對象初始化）。
// __fastcall 函數的前兩個參數不通過棧（Stack）傳遞，而是直接放入 CPU 的 ECX 和 EDX 暫存器。
// 在類別成員函數中，ECX 固定用來傳遞對象的指針（即 this 指針）。
// 因此，這裡的 CWvsApp* pThis 對應 ECX，而 void* _EDX 是一個佔位符（對應 EDX，通常在原版函數中沒被使用，但為了保持堆疊與暫存器平衡，必須寫出來）。
       
void __fastcall CWvsApp__ctor_hook(CWvsApp* pThis, void* _EDX, const char* sCmdLine) {
    DEBUG_MESSAGE("CWvsApp::CWvsApp");
    CWvsApp::ms_pInstance = pThis;
    pThis->m_hWnd = nullptr;
    pThis->m_bPCOMInitialized = 0;
    pThis->m_hHook = nullptr;
    pThis->m_nOSVersion = 0;
    pThis->m_nOSMinorVersion = 0;
    pThis->m_nOSBuildNumber = 0;


    // 語法/作用：這裡使用了顯式構造（Placement New 的變體）。
    // 深層解釋：在 C++ 中，如果一個類別成員是複雜的對象（如 ZXString 字符串類別、ZArray 陣列類別，這些都是遊戲自帶的封裝類別），
    // 它們有自己的構造函數。因為我們是在 Hook 函數裡「手動」操作這塊內存，直接賦值會出錯。
    // 這裡傳入變量的地址（如 &pThis->m_sCSDVersion），強行觸發該結構的初始構造函數，為其內部指針、長度等分配合理的初始內存。
    construct<ZXString<char>>(&pThis->m_sCSDVersion);
    pThis->m_b64BitInfo = 0;
    pThis->m_tUpdateTime = 0;
    pThis->m_bFirstUpdate = 1;
    construct<ZXString<char>>(&pThis->m_sCmdLine);
    pThis->m_nGameStartMode = 0;
    pThis->m_bAutoConnect = 1;
    pThis->m_bShowAdBalloon = 0;
    pThis->m_bExitByTitleEscape = 0;
    pThis->m_hrZExceptionCode = 0;
    pThis->m_hrComErrorCode = 0;
    pThis->m_tNextSecurityCheck = 0;
    pThis->m_bEnabledDX9 = true;
    construct<ZArray<uint8_t>>(&pThis->m_pBackupBuffer);
    pThis->m_dwBackupBufferSize = 0;
    pThis->m_dwClearStackLog = 0;
    pThis->m_bWindowActive = 1;

    // 語法/作用：調用剛才構造好的 ZArray 陣列對象的 Alloc 方法，在堆（Heap）上分派 0x1000 字節（4KB）的內存空間
    // 這個 Buffer 通常用於處理封包備份或防護緩衝。
    pThis->m_pBackupBuffer.Alloc(0x1000);
    pThis->m_nGameStartMode = 2;

    // 語法/作用：調用 Windows API 獲取當前執行緒的 ID，並記錄為主執行緒。這在遊戲後續的訊息循環（Message Loop）中非常重要，
    // 用來判斷某些操作是不是在主執行緒中安全執行。
    pThis->m_dwMainThreadId = GetCurrentThreadId();


    // 語法/作用：調用 Windows 舊 API 獲取當前運行的操作系統版本。
    // 歷史包袱：如果 dwPlatformId == 1（即 VER_PLATFORM_WIN32_WINDOWS），說明是古老的 Win95/Win98/WinMe。
    // 這行代碼給 m_bWin9x 標記賦值，後續遊戲會根據這個標記走不同的老舊相容代碼。
    OSVERSIONINFOA ovi;
    ovi.dwOSVersionInfoSize = 148;
    GetVersionExA(&ovi);
    pThis->m_bWin9x = ovi.dwPlatformId == 1;

    // 語法/作用：dwMajorVersion >= 6 代表 Windows Vista / Win7 / Win10 / Win11。如果是這些較新的系統，且啟動模式為 0，
    // 則再度強制修正為 2，防止新系統的安全機制阻攔遊戲。
    if (ovi.dwMajorVersion >= 6 && !pThis->m_nGameStartMode) {
        pThis->m_nGameStartMode = 2;
    }

    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    auto fnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(GetModuleHandleA("KERNEL32"), "IsWow64Process"));

    // 語法/作用：如果成功拿到了函數地址（指針不為空），就調用它。GetCurrentProcess() 獲取當前遊戲進程控制代碼，
    // 檢測它是不是運行在 64 位元系統的 WOW64（32位元模擬器）環境下。結果會寫入 bIs64。
    BOOL bIs64 = 0;
    if (fnIsWow64Process) {
        fnIsWow64Process(GetCurrentProcess(), &bIs64);
    }

    // 語意：如果系統是 Win7 以上且居然是原生 32 位元系統，則去執行原版遊戲在 0x0045ECD0 處的分支代碼（註釋提示這可能是重置 LSP 網路分層的函數）。
    if (ovi.dwMajorVersion >= 6 && !bIs64) {
        // ResetLSP();

        // reinterpret_cast<void(__cdecl*)()>(0x0045ECD0)() 是一個非常硬核的逆向語法：
        // 它把內存絕對地址 0x0045ECD0 強制轉換成一個「無參數、無返回值、調用約定為 __cdecl 的函數指針」，
        // 並在末尾加上 () 立即執行它!!。
        reinterpret_cast<void(__cdecl*)()>(0x0045ECD0)();  // ResetLSP 方法
    }

    // CWvsApp::SetClearStackLog(this, (bIs64 << 8) + (m_nOSVersion << 12));
    // 第一個參數傳入 pThis（手動模擬 this 指針），第二個參數是一個經過位移運算（Bitwise Shift）拼裝出來的 uint32_t 密碼子。
    // 這一步是在調用原版的 SetClearStackLog 函數，通知原版內核當前系統的架構特徵。
    reinterpret_cast<void(__thiscall*)(CWvsApp*, uint32_t)>(0x009C1960)(pThis, (bIs64 << 8) + (pThis->m_nOSVersion << 12));
    pThis->m_nOSVersion = ovi.dwMajorVersion;
    pThis->m_nOSMinorVersion = ovi.dwMinorVersion;
    pThis->m_nOSBuildNumber = ovi.dwBuildNumber;
    pThis->m_sCSDVersion = ovi.szCSDVersion;
    pThis->m_b64BitInfo = bIs64;
}


static auto CWvsApp__SetUp = 0x009CAFB0;

//作用：接管遊戲啟動時的數據加載與窗口創建初始化（SetUp 階段）。
void __fastcall CWvsApp__SetUp_hook(CWvsApp* pThis, void* _EDX) {
    DEBUG_MESSAGE("CWvsApp::SetUp");
    // CWvsApp::InitializeAuth(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C3AD0)(pThis); // CWvsApp::InitializeAuth
    srand(timeGetTime());
    // GetSEPrivilege();
    reinterpret_cast<void(__cdecl*)()>(0x0045E030)();

    DEBUG_MESSAGE("CWvsApp::SetUp - Initializing...");
    // TSingleton<CConfig>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C2420)();
    // CWvsApp::InitializePCOM(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C16C0)(pThis);
    // CWvsApp::CreateMainWindow(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C74A0)(pThis);

    DEBUG_MESSAGE("CWvsApp::SetUp - Connecting to server...");
    // TSingleton<CClientSocket>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C23A0)();
    // CWvsApp::ConnectLogin(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C1B30)(pThis);
    // TSingleton<CFuncKeyMappedMan>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C2510)();
    // TSingleton<CQuickslotKeyMappedMan>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C27E0)();
    // TSingleton<CMacroSysMan>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C2590)();
    // TSingleton<CBattleRecordMan>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C2220)();
    // CWvsApp::InitializeResMan(this);
    pThis->InitializeResMan();

    DEBUG_MESSAGE("CWvsApp::SetUp - Graphic & Sound...");
    // CWvsApp::InitializeGr2D(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C7670)(pThis);
    // TSingleton<CInputSystem>::CreateInstance();
    reinterpret_cast<void(__cdecl*)()>(0x009C7C30)();
    // CInputSystem::Init(CInputSystem::GetInstance(), m_hWnd, m_ahInput);
    reinterpret_cast<void(__thiscall*)(CInputSystem*, HWND, void**)>(0x00571A60)(CInputSystem::GetInstance(), pThis->m_hWnd, pThis->m_ahInput);

    ShowWindow(pThis->m_hWnd, SW_SHOW);
    UpdateWindow(pThis->m_hWnd);
    SetForegroundWindow(pThis->m_hWnd);
    get_gr()->RenderFrame();
    // CWvsApp::InitializeSound(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009CA170)(pThis);

    DEBUG_MESSAGE("CWvsApp::SetUp - Loading Data...");
    // CWvsApp::InitializeGameData(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C8440)(pThis);
    // TSingleton<CQuestMan>::CreateInstance()->LoadDemand();
    auto pQuestMan = reinterpret_cast<void*(__cdecl*)()>(0x009C21A0)();
    if (!reinterpret_cast<int32_t(__thiscall*)(void*)>(0x006C3D60)(pQuestMan)) {
        ErrorMessage("Failed to load quest data.");
    }
    // CQuestMan::LoadPartyQuestInfo(pQuestMan);
    reinterpret_cast<int32_t(__thiscall*)(void*)>(0x006C5540)(pQuestMan);
    // CQuestMan::LoadExclusive(pQuestMan);
    reinterpret_cast<int32_t(__thiscall*)(void*)>(0x006B9670)(pQuestMan);

    DEBUG_MESSAGE("CwvsApp::SetUp - Complete!");
    // TSingleton<CMonsterBookMan>::CreateInstance()->LoadBook();
    auto pMonsterBookMan = reinterpret_cast<void*(__cdecl*)()>(0x009CA820)();
    if (!reinterpret_cast<int32_t(__thiscall*)(void*)>(0x00664C10)(pMonsterBookMan)) {
        ErrorMessage("Failed to load monster book data.");
    }
    // CWvsApp::CreateWndManager(this);
    reinterpret_cast<void(__thiscall*)(CWvsApp*)>(0x009C2060)(pThis);
    // CConfig::ApplySysOpt(TSingleton<CConfig>::GetInstance(), nullptr, 0);
    reinterpret_cast<void(__thiscall*)(CConfig*, CONFIG_SYSOPT*, int)>(0x004B2300)(CConfig::GetInstance(), nullptr, 0);
    // TSingleton<CActionMan>::CreateInstance()->Init();
    auto pActionMan = reinterpret_cast<void*(__cdecl*)()>(0x009C22A0)();
    reinterpret_cast<void(__thiscall*)(void*)>(0x0041BEB0)(pActionMan);
    // TSingleton<CAnimationDisplayer>::CreateInstance();
    reinterpret_cast<void*(__cdecl*)()>(0x009C2320)();
    // TSingleton<CMapleTVMan>::CreateInstance()->Init()
    auto pMapleTVMan = reinterpret_cast<void*(__cdecl*)()>(0x009C2680)();
    reinterpret_cast<void(__thiscall*)(void*)>(0x0060FBC0)(pMapleTVMan);
    // TSingleton<CRadioManager>::CreateInstance();
    reinterpret_cast<void*(__cdecl*)()>(0x009C2770)();

    // (CLogo*) operator new(0x48); -> (CLogin*) operator new(0x2C8);
    CStage* pStage = static_cast<CStage*>(ZAllocEx<ZAllocAnonSelector>::s_Alloc(0x2C8));
    if (pStage) {
        // CLogo::CLogo(pStage); -> CLogin::Clogin(pStage);
        reinterpret_cast<void(__thiscall*)(void*)>(0x005DB440)(pStage);
    }
    set_stage(pStage, nullptr);
}


static auto CWvsApp__Run = 0x009C5F00;
//作用：遊戲主邏輯的幀更新/計時器回調。
void __fastcall CWvsApp__CallUpdate_hook(CWvsApp* pThis, void* _EDX, int32_t tCurTime) {
    if (pThis->m_bFirstUpdate) {
        pThis->m_tUpdateTime = tCurTime;
        pThis->m_tLastServerIPCheck = tCurTime;
        pThis->m_tLastServerIPCheck2 = tCurTime;
        pThis->m_tLastGGHookingAPICheck = tCurTime;
        pThis->m_tLastSecurityCheck = tCurTime;
        pThis->m_bFirstUpdate = 0;
    }
    while (tCurTime - pThis->m_tUpdateTime > 0) {
        auto pStage = get_stage();
        if (pStage) {
            pStage->Update();
        }
        // CWndMan::s_Update();
        reinterpret_cast<void(__cdecl*)()>(0x009B4B00)();
        pThis->m_tUpdateTime += 30;
        if (tCurTime - pThis->m_tUpdateTime > 0) {
            get_gr()->UpdateCurrentTime(pThis->m_tUpdateTime);
        }
    }
    get_gr()->UpdateCurrentTime(tCurTime);
    // CActionMan::SweepCache(TSingleton<CActionMan>::GetInstance());
    reinterpret_cast<void(__thiscall*)(CActionMan*)>(0x00415F60)(CActionMan::GetInstance());
}

// 作用：接管遊戲的最核心主循環（Message Loop）。
void __fastcall CWvsApp__Run_hook(CWvsApp* pThis, void* _EDX, int32_t* pbTerminate) {
    HRESULT hr;
    MSG msg;
    ISMSG isMsg;
    memset(&msg, 0, sizeof(msg));
    memset(&isMsg, 0, sizeof(isMsg));
    if (CClientSocket::IsInstantiated()) {
        // CClientSocket::ManipulatePacket(TSingleton<CClientSocket>::GetInstance());
        reinterpret_cast<void(__thiscall*)(CClientSocket*)>(0x004B0220)(CClientSocket::GetInstance());
    }
    do {
        DWORD dwRet = MsgWaitForMultipleObjects(3, pThis->m_ahInput, 0, 0, 0xFF);
        if (dwRet <= 2) {
            // CInputSystem::UpdateDevice(TSingleton<CInputSystem>::GetInstance(), dwRet);
            reinterpret_cast<void(__thiscall*)(CInputSystem*, int32_t)>(0x00571710)(CInputSystem::GetInstance(), dwRet);
            do {
                // if (!CInputSystem::GetISMessage(TSingleton<CInputSystem>::GetInstance(), &isMsg))
                if (!reinterpret_cast<int32_t(__thiscall*)(CInputSystem*, ISMSG*)>(0x005708F0)(CInputSystem::GetInstance(), &isMsg)) {
                    break;
                }
                // CWvsApp::ISMsgProc(this, isMsg.message, isMsg.wParam, isMsg.lParam);
                reinterpret_cast<void(__thiscall*)(CWvsApp*, uint32_t, uint32_t, int32_t)>(0x009C1CE0)(pThis, isMsg.message, isMsg.wParam, isMsg.lParam);
            } while (!*pbTerminate);
        } else if (dwRet == 3) {
            do {
                if (!PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
                // if (CWvsApp::ExtractComErrorCode(this, &hr))
                if (reinterpret_cast<int32_t(__thiscall*)(CWvsApp*, HRESULT*)>(0x009C0860)(pThis, &hr)) {
                    _com_issue_error(hr);
                }
                // if (CWvsApp::ExtractZExceptionCode(this, &hr))
                if (reinterpret_cast<int32_t(__thiscall*)(CWvsApp*, HRESULT*)>(0x009C0830)(pThis, &hr)) {
                    ZException exception(hr);
                    if (hr == 0x20000000) {
                        // CPatchException::CPatchException(&exception, m_nTargetVersion);
                        reinterpret_cast<void(__thiscall*)(void*, int32_t)>(0x00520FA0)(&exception, pThis->m_nTargetVersion);
                    } else if (hr >= 0x21000000 && hr <= 0x21000006) {
                        // CDisconnectException::CDisconnectException(&exception, hr);
                        reinterpret_cast<void(__thiscall*)(void*, HRESULT)>(0x00429860)(&exception, hr);
                    } else if (hr >= 0x22000000 && hr <= 0x2200000E) {
                        // CTerminateException::CTerminateException(&exception, hr);
                        reinterpret_cast<void(__thiscall*)(void*, HRESULT)>(0x00401D50)(&exception, hr);
                    }
                    throw exception;
                }
            } while (!*pbTerminate && msg.message != WM_QUIT);
        } else {
            // if (CInputSystem::GenerateAutoKeyDown(TSingleton<CInputSystem>::GetInstance(), &isMsg))
            if (reinterpret_cast<int32_t(__thiscall*)(CInputSystem*, ISMSG*)>(0x0056F990)(CInputSystem::GetInstance(), &isMsg)) {
                // CWvsApp::ISMsgProc(this, isMsg.message, isMsg.wParam, isMsg.lParam);
                reinterpret_cast<void(__thiscall*)(CWvsApp*, uint32_t, uint32_t, int32_t)>(0x009C1CE0)(pThis, isMsg.message, isMsg.wParam, isMsg.lParam);
            }
            // if (CInputSystem::GenerateAutoBtnDown(TSingleton<CInputSystem>::GetInstance(), &isMsg))
            if (reinterpret_cast<int32_t(__thiscall*)(CInputSystem*, ISMSG*)>(0x0056FAC0)(CInputSystem::GetInstance(), &isMsg)) {
                // CWvsApp::ISMsgProc(this, isMsg.message, isMsg.wParam, isMsg.lParam);
                reinterpret_cast<void(__thiscall*)(CWvsApp*, uint32_t, uint32_t, int32_t)>(0x009C1CE0)(pThis, isMsg.message, isMsg.wParam, isMsg.lParam);
            }
            int32_t tCurTime = get_gr()->nextRenderTime;
            CWvsApp__CallUpdate_hook(pThis, _EDX, tCurTime);
            // CWndMan::RedrawInvalidatedWindows();
            reinterpret_cast<void(__cdecl*)()>(0x009B2340)();
            get_gr()->RenderFrame();
            Sleep(1);
        }
    } while (!*pbTerminate && msg.message != WM_QUIT);
    if (msg.message == WM_QUIT) {
        PostQuitMessage(0);
    }
}


static auto CClientSocket__Connect = 0x004B0340;

// 作用：接管客戶端連接服務器的底層操作。
void __fastcall CClientSocket__Connect_hook(CClientSocket* pThis, void* _EDX, CClientSocket::CONNECTCONTEXT* ctx) {
    DEBUG_MESSAGE("CClientSocket::Connect");
    pThis->m_ctxConnect.lAddr.RemoveAll();
    pThis->m_ctxConnect.lAddr.AddTail(ctx->lAddr);
    pThis->m_ctxConnect.posList = ctx->posList;
    pThis->m_ctxConnect.bLogin = ctx->bLogin;
    pThis->m_ctxConnect.posList = pThis->m_ctxConnect.lAddr.GetHeadPosition();
    auto next = ZList<ZInetAddr>::GetNext(pThis->m_ctxConnect.posList);

    DEBUG_MESSAGE("CClientSocket::Connect (addr)");
    // CClientSocket::ClearSendReceiveCtx(this);
    reinterpret_cast<void(__thiscall*)(CClientSocket*)>(0x004AE1A0)(pThis);
    // ZSocketBase::CloseSocket(&m_sock);
    reinterpret_cast<void(__thiscall*)(ZSocketBase*)>(0x004ACF30)(&pThis->m_sock);
    // ZSocketBase::Socket(&m_sock, 1, 2, 0);
    reinterpret_cast<void(__thiscall*)(ZSocketBase*, int, int, int)>(0x004ACF50)(&pThis->m_sock, 1, 2, 0);
    // CClientSocket::SetTimeout(this);
    reinterpret_cast<void(__thiscall*)(CClientSocket*)>(0x004ACBA0)(pThis);


    // 通訊模式重寫：調用 WSAAsyncSelect 將 Socket 設為非阻塞異步模式，
    // 並強制綁定 Windows 消息 0x401（通常是遊戲定義的網絡事件消息 ID），
    // 確保數據包能精準發往你自己的服務端後端，而不是官方的網關。
    if (WSAAsyncSelect(pThis->m_sock, pThis->m_hWnd, 0x401, 0x33) == -1 ||
            connect(pThis->m_sock, next, 16) != -1 ||
            WSAGetLastError() != WSAEWOULDBLOCK) {
        // CClientSocket::OnConnect(this, 0);
        reinterpret_cast<void(__thiscall*)(CClientSocket*, int)>(0x004AEF10)(pThis, 0);
    }
}

static auto CClientSocket__OnAliveReq = 0x004AFC90;
//作用：自動響應服務器的心跳包（Ping/KeepAlive）。
void __fastcall CClientSocket__OnAliveReq_hook(CClientSocket* pThis, void* _EDX, CInPacket& iPacket) {
    COutPacket oPacket(25); // CP_AliveAck
    pThis->SendPacket(oPacket);
}


static auto CLogin__SendCheckPasswordPacket = 0x005DB9D0;
// 作用：接管發送登錄賬號密碼的封包構造。
int32_t __fastcall CLogin__SendCheckPasswordPacket_hook(CLogin* pThis, void* _EDX, char* sID, char* sPasswd) {
    if (pThis->m_bRequestSent) {
        return 0;
    }
    pThis->m_bRequestSent = 1;
    pThis->m_WorldItem.RemoveAll();
    pThis->m_aBalloon.RemoveAll();

    CSystemInfo si;
    // CSystemInfo::Init(&si);
    reinterpret_cast<void(__thiscall*)(CSystemInfo*)>(0x00A1F1B0)(&si);

    COutPacket oPacket(1); // CP_CheckPassword
    oPacket.EncodeStr(sID);
    oPacket.EncodeStr(sPasswd);
    oPacket.EncodeBuffer(si.MachineId, 16);
    oPacket.Encode4(0); // CSystemInfo::GetGameRoomClient(&v15)
    oPacket.Encode1(CWvsApp::GetInstance()->m_nGameStartMode);
    oPacket.Encode1(0);
    oPacket.Encode1(0);
    oPacket.Encode4(0); // CConfig::GetPartnerCode(TSingleton<CConfig>::ms_pInstance._m_pStr)
    CClientSocket::GetInstance()->SendPacket(oPacket);
    return 0;
}


static auto CWvsContext__OnEnterField = 0x009DBEC0;
// 作用：當玩家進入新地圖（切換地圖/進遊戲）時觸發。
void __fastcall CWvsContext__OnEnterField_hook(CWvsContext* pThis, void* _EDX) {
    // CWvsContext::UI_CloseRevive(this);
    reinterpret_cast<void(__thiscall*)(CWvsContext*)>(0x009CCCD0)(pThis);
    // CTemporaryStatView::Show(&m_temporaryStatView);
    reinterpret_cast<void(__thiscall*)(CTemporaryStatView*)>(0x0075C6A0)(&pThis->m_temporaryStatView);
    pThis->m_bKillMobFromEnterField = 0;
}


static auto CInputSystem__DetectJoystick = 0x00571740;
//作用：探測遊戲手柄/搖桿。
void __fastcall CInputSystem__DetectJoystick_hook(CInputSystem* pThis, void* _EDX) {
    // noop
}


static auto CCtrlComboBox__AddItem = 0x004DE640;
//CCtrlComboBox__AddItem_hooks
void __fastcall CCtrlComboBox__AddItem_hook(CCtrlComboBox* pThis, void* _EDX, char* sItemName, uint32_t dwParam) {
    pThis->AddItem("To Spouse", 0x6);   // ID_CHAT_TARGET_COUPLE
    pThis->AddItem("Whisper", 0x7);     // ID_CHAT_TARGET_WHISPER
    pThis->AddItem(sItemName, dwParam); // overwritten call for ID_CHAT_TARGET_ALL
}

//它調用 ATTACH_HOOK 宏，把遊戲原版的構造函數、啟動邏輯、主循環、網絡連接等關鍵函數替換為下面定義的 Hook 函數。
// 包含一系列 PatchRetZero（修改內存讓函數直接返回 0），用於屏蔽原版的安全保護和干擾組件：
// DR_check / SendHSLog / CeTracer::Run：屏蔽 HackShield（反作弊系統）的檢測、日誌發送和追蹤。
// Hidedll：防止客戶端隱藏自身 DLL，方便調試。ShowStartUpWndModal /ShowAdBalloon：屏蔽遊戲啟動時的彈窗廣告或氣球廣告。
// CWvsApp::EnableWinKey：防止遊戲屏蔽系統的 Windows 鍵。
// 劫持 CCtrlComboBox__AddItem 用於修復聊天框下拉菜單。


void AttachClientBypass() {
    ATTACH_HOOK(CWvsApp__ctor, CWvsApp__ctor_hook);
    ATTACH_HOOK(CWvsApp__SetUp, CWvsApp__SetUp_hook);
    ATTACH_HOOK(CWvsApp__Run, CWvsApp__Run_hook);
    ATTACH_HOOK(CClientSocket__Connect, CClientSocket__Connect_hook);
    ATTACH_HOOK(CClientSocket__OnAliveReq, CClientSocket__OnAliveReq_hook);
    ATTACH_HOOK(CLogin__SendCheckPasswordPacket, CLogin__SendCheckPasswordPacket_hook);
    ATTACH_HOOK(CWvsContext__OnEnterField, CWvsContext__OnEnterField_hook);
    ATTACH_HOOK(CInputSystem__DetectJoystick, CInputSystem__DetectJoystick_hook);

    // CUIStatusBar::MakeCtrlEdit - add missing combo box items : "To Spouse", "Whisper"
    PatchCall(0x00870E82, reinterpret_cast<uintptr_t>(&CCtrlComboBox__AddItem_hook));

    // 3. 安全防護層：全面過白與降級（Bypass）
    // 這是最重要的一步。 如果不改變客戶端的安全檢測，客戶端只要一發現連線 IP 不是官方的，
    // 或者發現內存被修改（比如有這個 DLL 存在），就會直接彈窗報錯或閃退。

    PatchRetZero(0x004AB900); // DR_check  (反作弊核心主線程)
    PatchRetZero(0x0045EBD0); // Hidedll
    PatchRetZero(0x009BF6C0); // SendHSLog   (發送作弊日誌)
    PatchRetZero(0x009BF370); // CeTracer::Run  (內存追蹤保護)
    PatchRetZero(0x009BF390); // ShowStartUpWndModal
    PatchRetZero(0x00429000); // ShowAdBalloon
    PatchRetZero(0x009CC220); // CWvsApp::EnableWinKey
}