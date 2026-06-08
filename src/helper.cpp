#include "pch.h"
#include "hook.h"
#include "ztl/ztl.h"
#include "common/vecctrl.h"
#include "common/iteminfo.h"
#include "common/skillinfo.h"
#include "wvs/util.h"
#include "wvs/userlocal.h"
#include "wvs/inputsystem.h"
#include "wvs/animationdisplayer.h"
#include <cstdint>
#include <imm.h>
#pragma comment(lib, "imm32.lib")



static auto CUserLocal__Jump = reinterpret_cast<void(__thiscall*)(CUserLocal*, int32_t)>(0x0090A1D0);

void __fastcall CUserLocal__Jump_hook(CUserLocal* pThis, void* _EDX, int32_t bEnforced) {
    CUserLocal__Jump(pThis, bEnforced);
    CVecCtrl* pVecCtrl = reinterpret_cast<CVecCtrl*>(&static_cast<IWzVector2D*>(pThis->m_pvc)[-3]);
    // CVecCtrl::GetFoothold(pVecCtrl) || CVecCtrl::GetLadderOrRope(pVecCtrl)
    if (reinterpret_cast<CStaticFoothold*(__thiscall*)(CVecCtrl*)>(0x00639F20)(pVecCtrl) ||
            reinterpret_cast<CLadderOrRope*(__thiscall*)(CVecCtrl*)>(0x004BBE80)(pVecCtrl)) {
        return;
    }
    if (!bEnforced && pThis->m_bJumpKeyUp) {
        int32_t nJob = pThis->GetJobCode();
        int32_t nSkillID = 0;
        if (nJob / 10 == 41) {
            nSkillID = 4111006; // Night Lord
        } else if (nJob / 10 == 42) {
            nSkillID = 4211009; // Shadower
        } else if (nJob / 10 == 43) {
            nSkillID = 4321003; // Dual Blade
        } else if (nJob / 100 == 14) {
            nSkillID = 14101004; // Night Walker
        }
        if (nSkillID) {
            pThis->m_bJumpKeyUp = false;
            pThis->DoActiveSkill(nSkillID, 0, nullptr);
        }
    }
}


static auto CVecCtrl__SetImpactNext = reinterpret_cast<void(__thiscall*)(CVecCtrl*, int32_t, double, double)>(0x00905CD0);

void __fastcall CVecCtrl__SetImpactNext_hook(CVecCtrl* pThis, void* _EDX, int32_t nAttr, double vx, double vy) {
    int32_t nInputX;
    int32_t nInputY;
    // CVecCtrl::GetInput(pThis, nInputX, nInputY);
    reinterpret_cast<void(__thiscall*)(CVecCtrl*, int32_t&, int32_t&)>(0x009049C0)(pThis, nInputX, nInputY);
    if (nAttr == 0x14 && nInputY < 0) {
        // MPA_FLASHJUMP
        vx = 0.0;
        vy = -1000.0;
    }
    CVecCtrl__SetImpactNext(pThis, nAttr, vx, vy);
}


static auto CItemInfo__GetMapString = reinterpret_cast<ZXString<char>*(__thiscall*)(CItemInfo*, ZXString<char>*, uint32_t, const char*)>(0x005A9CA0);

ZXString<char>* __fastcall CItemInfo__GetMapString_hook(CItemInfo* pThis, void* _EDX, ZXString<char>* result, uint32_t dwFieldID, const char* sKey) {
    CItemInfo__GetMapString(pThis, result, dwFieldID, sKey);
    if (!strcmp(sKey, "mapName")) {
        ZXString<char> sFieldID;
        sFieldID.Format(" (%d)", dwFieldID);
        result->Cat(sFieldID);
    }
    return result;
}

static auto CItemInfo__GetItemDesc = reinterpret_cast<ZXString<char>*(__thiscall*)(CItemInfo*, ZXString<char>*, int32_t)>(0x005B16E0);

ZXString<char>* __fastcall CItemInfo__GetItemDesc_hook(CItemInfo* pThis, void* _EDX, ZXString<char>* result, int32_t nItemID) {
    CItemInfo__GetItemDesc(pThis, result, nItemID);
    if (result->GetLength() > 0) {
        result->Cat("\r\n");
    }
    ZXString<char> sItemID;
    sItemID.Format("#cItem ID : %d#", nItemID);
    result->Cat(sItemID);
    return result;
}

static auto CSkillInfo__LoadSkill = reinterpret_cast<ZRef<SKILLENTRY>*(__thiscall*)(CSkillInfo*, ZRef<SKILLENTRY>*, int32_t, IWzPropertyPtr, IWzPropertyPtr)>(0x0070C190);

ZRef<SKILLENTRY>* __fastcall CSkillInfo__LoadSkill_hook(CSkillInfo* pThis, void* _EDX, ZRef<SKILLENTRY>* result, int32_t nSkillID, IWzPropertyPtr pSkill, IWzPropertyPtr pStrSR) {
    CSkillInfo__LoadSkill(pThis, result, nSkillID, pSkill, pStrSR);
    auto entry = *result;
    if (entry->sDescription.GetLength() > 0) {
        entry->sDescription.Cat("\r\n");
    }
    ZXString<char> sSkillID;
    sSkillID.Format("#cSkill ID : %d#", nSkillID);
    entry->sDescription.Cat(sSkillID);
    return result;
}

static auto get_weapon_attack_speed = 0x005A66B0;

const char* get_attack_speed_string(int32_t nAttackSpeed) {
    switch (nAttackSpeed) {
    case 0:
    case 1:
        return " FASTEST";
    case 2:
    case 3:
        return " FASTER";
    case 4:
    case 5:
        return " FAST";
    case 6:
        return " NORMAL";
    case 7:
    case 8:
        return " SLOW";
    case 9:
    case 10:
        return " SLOWER";
    case 11:
    case 12:
        return " SLOWEST";
    default:
        return "";
    }
}

ZXString<char>* __cdecl get_weapon_attack_speed_hook(ZXString<char>* result, int32_t nItemID) {
    construct(result);
    // get_weapon_type(nItemID)
    if (reinterpret_cast<int32_t(__cdecl*)(int32_t)>(0x0046F660)(nItemID)) {
        ZXString<wchar_t> sUOL;
        // get_equip_data_path(&sUOL, nItemID)
        reinterpret_cast<ZXString<wchar_t>*(__cdecl*)(ZXString<wchar_t>*, int32_t)>(0x005A6060)(&sUOL, nItemID);
        if (!sUOL.IsEmpty()) {
            IWzPropertyPtr pEquip = get_rm()->GetObjectA(Ztl_bstr_t(sUOL)).GetUnknown();
            IWzPropertyPtr pInfo = pEquip->item[L"info"].GetUnknown();
            uint32_t nAttackSpeed = get_int32(pInfo->item[L"attackSpeed"], 6);
            result->Format("%s (%d)", get_attack_speed_string(nAttackSpeed), nAttackSpeed);
        }
    }
    return result;
}

static uintptr_t CUIQuestInfoDetail__Draw_jmp = 0x00824A93;
static uintptr_t CUIQuestInfoDetail__Draw_ret = 0x00824C04;

void __stdcall CUIQuestInfoDetail__Draw_helper(IWzCanvas* pCanvas, IWzFont* pFont, uint16_t usQuestID) {
    wchar_t sQuestID[256];
    swprintf_s(sQuestID, 256, L"Quest ID : %d", usQuestID);
    pCanvas->DrawTextA(35, 56, sQuestID, pFont);
}

void __declspec(naked) CUIQuestInfoDetail__Draw_hook() {
    __asm {
        mov     eax, [ ebp + 0x8C ]
        movzx   edx, word ptr [ eax ]
        push    edx                     ; this->m_pQI.p->usQuestID
        mov     eax, [ ebp + 0xF0 ]
        push    eax                     ; IWzFont*
        push    esi                     ; IWzCanvas*
        call    CUIQuestInfoDetail__Draw_helper
        jmp     [ CUIQuestInfoDetail__Draw_ret ]
    }
}




// IME-单行输入框能用，且不卡门、排除密码框-以下-江奈Mizuki
// DisableIme()来自Beidou-Cosmic
void DisableIme() {
    HWND hwnd = GetForegroundWindow(); // 获取当前前台窗口的句柄
    if (hwnd) {
        // 获取输入法上下文
        HIMC hImc = ImmGetContext(hwnd);
        if (hImc) {
            // 解除输入法上下文的关联
            ImmAssociateContext(hwnd, NULL);
            ImmReleaseContext(hwnd, hImc);
        }
    }
}

BYTE enabled = 1; // IME是否启用的标记,有一个疑问，这个helper.cpp函数并没有#program once 在这里定义一个标记是否合适？是不是另外新建一个cpp文件写比较好？
// destroyWindow_GMS095()修改自Beidou-Cosmic的destroyWindow

// CCtrlWnd::Destroy
/*
void __thiscall CCtrlWnd::Destroy(CCtrlWnd *this)
{
  IWzVector2D *m_pInterface; // eax

  if ( this->m_nCtrlId != -1 )
  {
    CWndMan::Unlink(TSingleton<CWndMan>::ms_pInstance, &this->IUIMsgHandler);
    this->OnDestroy(this);
    CWnd::RemoveChild(this->m_pParent, this);
    this->m_pParent = 0;
    m_pInterface = this->m_pLTCtrl.m_pInterface;
    if ( m_pInterface )
    {
      this->m_pLTCtrl.m_pInterface = 0;
      m_pInterface->Release(m_pInterface);
    }
    this->m_nCtrlId = -1;
  }
}
*/


DWORD destroyWindowRtnAddr = 0x004F0268;
__declspec(naked) void destroyWindow_GMS095() {
    __asm {
		mov dword ptr [esi+14h], 0FFFFFFFFh      // 原本GMS095的CCtrlWnd::Destroy中的this->m_nCtrlId = -1;
 
		cmp enabled, 0
		jz label_return                      // 如果IME已经禁用了，就不必禁用了
 
		call DisableIme
		mov enabled, 0 // 如果IME还没禁用，就禁用，并把enabled设置成1
 
		label_return :
		jmp destroyWindowRtnAddr // 补丁执行完了，贴回去。
    }
}

// SingleLineIME()实现了GMS095版本的激活单行输入框的输入法，并且是密码栏的时候不会调出输入法-江奈Mizuki
DWORD EnableIMEAttr = 0x009B4D50; //void __thiscall CWndMan::EnableIME(CWndMan *this, int bEnable)
DWORD SingleLineIMERtnAttr = 0x004DE8F0;
__declspec(naked) void SingleLineIME() {
    __asm {
		cmp[esi + 0x80], 1 // 判断是否是密码框。在095IDB里CCtrlEdit的结构体里看到偏移0x84是int m_bPasswd，而CCtrlEdit::OnSetFocus函数传进来的应该是 IUIMsgHandler* this，所以所有调用本地变量地址的偏移量都会偏差0x4
        jz label_disable                // 如果是密码框直接disable
        push 1
        call EnableIMEAttr // 以1为参数调用void __thiscall CWndMan::EnableIME(CWndMan *this, int bEnable)
        mov enabled, 1
        jmp SingleLineIMERtnAttr       // 补丁执行完了，贴回去。
 
        label_disable :
		call DisableIme
        mov enabled, 0
		jmp SingleLineIMERtnAttr       // 补丁执行完了，贴回去。

    }
}

// 安装 IME 补丁
void InstallImePatch() {

    PatchJmp(0x004DE8E9, reinterpret_cast<uintptr_t>(SingleLineIME));
    PatchJmp(0x004F0261, reinterpret_cast<uintptr_t>(destroyWindow_GMS095));

    // IsDBCSLeadByte(*Buffer) 
    PatchNop(0x00880001, 0x00880006); // 聊天支持中文-按下回车的检测

    // IsDBCSLeadByte(*v7)
    PatchNop(0x008E87CE, 0x008E87D3); // 聊天支持中文-对消息的检测

    // is_valid_character_name

    PatchNop(0x007475E4, 0x007475EA); // 角色名支持中文
}

void AttachClientHelper() {
    // EqSlotInfo sEqSlotInfo[BP_PETWEAR] - fix pet equip slot position
    Patch4(0x00C614C0, 110);
    Patch4(0x00C614C4, 89);

    // CChatHelper::TryChat
    Patch1(0x004AA7EF, 0xEB); // bypass chat cooldown
    Patch1(0x004AA74A, 0xEB); // bypass chat repeat

    // CCurseProcess::ProcessString
    PatchStr(0x007477F0, "\x31\xC0\x40\xC3"); // disable profanity filter

    // CVecCtrl::IsFalling
    PatchRetZero(0x0093A140); // double jump while falling

    // CUserLocal::UseFuncKeyMapped
    PatchCall(0x009336D3, reinterpret_cast<uintptr_t>(&CUserLocal__Jump_hook));

    ATTACH_HOOK(CVecCtrl__SetImpactNext, CVecCtrl__SetImpactNext_hook); // vertical double jump
    ATTACH_HOOK(CItemInfo__GetMapString, CItemInfo__GetMapString_hook); // append map ID to map name
    ATTACH_HOOK(CItemInfo__GetItemDesc, CItemInfo__GetItemDesc_hook);   // append item ID to item description
    ATTACH_HOOK(CSkillInfo__LoadSkill, CSkillInfo__LoadSkill_hook);     // append skill ID to skill description
    ATTACH_HOOK(get_weapon_attack_speed, get_weapon_attack_speed_hook); // append attack speed value to weapon speed string

    PatchJmp(CUIQuestInfoDetail__Draw_jmp, reinterpret_cast<uintptr_t>(&CUIQuestInfoDetail__Draw_hook)); // replace "Low Level Quest"

    // 中文输入法
    InstallImePatch();
}