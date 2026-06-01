#include "hook.h"
#include "wvs/wvscontext.h"

 
UCHAR QuickslotKeyMapDefault[] = {
        42, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        4, 0, 0, 0,
        5, 0, 0, 0,
        6, 0, 0, 0,
        7, 0, 0, 0,
        30, 0, 0, 0,
        31, 0, 0, 0,
        32, 0, 0, 0,
        33, 0, 0, 0,
        34, 0, 0, 0,
        35, 0, 0, 0,
 
        29, 0, 0, 0,
        16, 0, 0, 0,
        17, 0, 0, 0,
        18, 0, 0, 0,
        19, 0, 0, 0,
        20, 0, 0, 0,
        21, 0, 0, 0,
        44, 0, 0, 0,
        45, 0, 0, 0,
        46, 0, 0, 0,
        47, 0, 0, 0,
        48, 0, 0, 0,
        49, 0, 0, 0,
};
 
UCHAR QuickslotKeyMap[] = {
        42, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        4, 0, 0, 0,
        5, 0, 0, 0,
        6, 0, 0, 0,
        7, 0, 0, 0,
        30, 0, 0, 0,
        31, 0, 0, 0,
        32, 0, 0, 0,
        33, 0, 0, 0,
        34, 0, 0, 0,
        35, 0, 0, 0,
 
        29, 0, 0, 0,
        16, 0, 0, 0,
        17, 0, 0, 0,
        18, 0, 0, 0,
        19, 0, 0, 0,
        20, 0, 0, 0,
        21, 0, 0, 0,
        44, 0, 0, 0,
        45, 0, 0, 0,
        46, 0, 0, 0,
        47, 0, 0, 0,
        48, 0, 0, 0,
        49, 0, 0, 0,
};
 
UCHAR QuickslotKeyMapOld[] = {
        42, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        4, 0, 0, 0,
        5, 0, 0, 0,
        6, 0, 0, 0,
        7, 0, 0, 0,
        30, 0, 0, 0,
        31, 0, 0, 0,
        32, 0, 0, 0,
        33, 0, 0, 0,
        34, 0, 0, 0,
        35, 0, 0, 0,
 
        29, 0, 0, 0,
        16, 0, 0, 0,
        17, 0, 0, 0,
        18, 0, 0, 0,
        19, 0, 0, 0,
        20, 0, 0, 0,
        21, 0, 0, 0,
        44, 0, 0, 0,
        45, 0, 0, 0,
        46, 0, 0, 0,
        47, 0, 0, 0,
        48, 0, 0, 0,
        49, 0, 0, 0,
};
 
UCHAR QuickSlotKeyPos[] = {
        7,   0, 0, 0, 15, 0, 0, 0,
        40,  0, 0, 0, 15, 0, 0, 0,
        73,  0, 0, 0, 15, 0, 0, 0,
        106, 0, 0, 0, 15, 0, 0, 0,
        139, 0, 0, 0, 15, 0, 0, 0,
        172, 0, 0, 0, 15, 0, 0, 0,
        205, 0, 0, 0, 15, 0, 0, 0,
        238, 0, 0, 0, 15, 0, 0, 0,
        15,  1, 0, 0, 15, 0, 0, 0,
        48,  1, 0, 0, 15, 0, 0, 0,
        81,  1, 0, 0, 15, 0, 0, 0,
        114, 1, 0, 0, 15, 0, 0, 0,
        147, 1, 0, 0, 15, 0, 0, 0,
        7,   0, 0, 0, 48, 0, 0, 0,
        40,  0, 0, 0, 48, 0, 0, 0,
        73,  0, 0, 0, 48, 0, 0, 0,
        106, 0, 0, 0, 48, 0, 0, 0,
        139, 0, 0, 0, 48, 0, 0, 0,
        172, 0, 0, 0, 48, 0, 0, 0,
        205, 0, 0, 0, 48, 0, 0, 0,
        238, 0, 0, 0, 48, 0, 0, 0,
        15,  1, 0, 0, 48, 0, 0, 0,
        48,  1, 0, 0, 48, 0, 0, 0,
        81,  1, 0, 0, 48, 0, 0, 0,
        114, 1, 0, 0, 48, 0, 0, 0,
        147, 1, 0, 0, 48, 0, 0, 0
};
 
UCHAR FuncKeyMappedInfo[312] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
 
UCHAR FuncKeyMappedSkillCooltime[104] = {
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255
};
 
void resetFuncKeyMappedInfo() {
    for (int i = 0; i < 312; ++i) {
        FuncKeyMappedInfo[i] = 0;
    }
}
 
 
DWORD ResetFKMInfo_Retn = 0x0086FB1F;
_declspec(naked) void ResetFKMInfo()
{
    _asm
    {
        pushad
        call resetFuncKeyMappedInfo
        popad
        mov  eax, [esi + 8Ch]
        jmp  ResetFKMInfo_Retn
    }
}
 
 
DWORD CompareValidate_Retn = 0x00871090;
_declspec(naked) void CompareValidateFuncKeyMappedInfo_cave()
{
    _asm
    {
        push 0x138
        push 0x0
        push ebx
        jmp CompareValidate_Retn
    }
}
 
 
DWORD QuickSlotBtOpenCcRtn = 0x0087540F;
_declspec(naked) void QuickSlotBtOpenCc()
{
    _asm {
        push 0    // y 偏移
        push 80  // x偏移
        push 1006 // id
        jmp QuickSlotBtOpenCcRtn
    }
}
 
DWORD QuickSlotBtCloseCcRtn = 0x0087547E;
_declspec(naked) void QuickSlotBtCloseCc()
{
    _asm {
        push 0    // y 偏移
        push 80  // x偏移
        push 1006 // id
        jmp QuickSlotBtCloseCcRtn
    }
}
 
DWORD QuickSlotXccRtn = 0x00875108;
_declspec(naked) void QuickSlotXcc()
{
    _asm {
        mov ecx, 358 // 800-442
        mov[esi + 8], ecx
        jmp QuickSlotXccRtn
    }
}
 
DWORD QuickslotKeyMapAddr = (DWORD)&QuickslotKeyMap;
 
DWORD CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc_rtn = 0x006C60DB;
void _declspec(naked) CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc()
{
    _asm {
        push 32 // 看情况是不是要改成 104，可能要配合服务端
        push QuickslotKeyMapAddr
        jmp CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc_rtn
    }
}
 
void copyQKMtoOld() {
    memcpy(QuickslotKeyMapOld, QuickslotKeyMap, sizeof(QuickslotKeyMapOld));
}
 
DWORD CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc_rtn = 0x006C6106;
void _declspec(naked) CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc()
{
    _asm {
        call copyQKMtoOld
        jmp CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc_rtn
    }
}
 
DWORD CQuickslotKeyMappedMan_OnInit_QKM_cc_rtn = 0x006C614A;
void _declspec(naked) CQuickslotKeyMappedMan_OnInit_QKM_cc()
{
    _asm {
        push 32 // 看情况是不是要改成 104，可能要配合服务端
        push QuickslotKeyMapAddr
        jmp CQuickslotKeyMappedMan_OnInit_QKM_cc_rtn
    }
}
 
DWORD CQuickslotKeyMappedMan_OnInit_Copy_cc_rtn = 0x006C6176;
void _declspec(naked) CQuickslotKeyMappedMan_OnInit_Copy_cc()
{
    _asm {
        call copyQKMtoOld
        jmp CQuickslotKeyMappedMan_OnInit_Copy_cc_rtn
    }
}
 
DWORD QuickslotKeyMapOldAddr = (DWORD)&QuickslotKeyMapOld;
DWORD IsQuickslotKeyMapModified_QKM_cc_rtn = 0x007D820B;
void _declspec(naked) IsQuickslotKeyMapModified_QKM_cc()
{
    _asm {
        mov edx, QuickslotKeyMapAddr
        mov eax, 104
        mov ecx, QuickslotKeyMapOldAddr
        jmp IsQuickslotKeyMapModified_QKM_cc_rtn
    }
}
 
void copyOldtoQKM() {
    memcpy(QuickslotKeyMap, QuickslotKeyMapOld, sizeof(QuickslotKeyMap));
}
 
DWORD CUIKeyConfig_Copy_cc_rtn = 0x007D8D36;
void _declspec(naked) CUIKeyConfig_Copy_cc()
{
    _asm {
        call copyOldtoQKM
        jmp  CUIKeyConfig_Copy_cc_rtn
    }
}
 
DWORD CompareValidateFuncKeyMappedInfo_FKM_cc_rtn = 0x008710F0;
void _declspec(naked) CompareValidateFuncKeyMappedInfo_FKM_cc()
{
    _asm {
        lea ebx, FuncKeyMappedInfo
        mov eax, [0x00C6AAE4]
        mov eax, [eax]
        jmp CompareValidateFuncKeyMappedInfo_FKM_cc_rtn
    }
}
 
DWORD CompareValidateFuncKeyMappedInfo_QKM_cc_rtn = 0x00871102;
void _declspec(naked) CompareValidateFuncKeyMappedInfo_QKM_cc()
{
    _asm {
        mov eax, QuickslotKeyMapAddr
        //add ebx, 0x94
        jmp CompareValidateFuncKeyMappedInfo_QKM_cc_rtn
    }
}
 
DWORD CompareValidateFuncKeyMappedInfo_FKM2_cc_rtn = 0x0087106C;
void _declspec(naked) CompareValidateFuncKeyMappedInfo_FKM2_cc()
{
    _asm {
        lea ecx, FuncKeyMappedInfo
        jmp CompareValidateFuncKeyMappedInfo_FKM2_cc_rtn
    }
}
 
DWORD Draw_QKM_cc_rtn = 0x008761D4;
void _declspec(naked) Draw_QKM_cc()
{
    _asm {
        mov ecx, QuickslotKeyMapAddr
        mov ecx, [ecx + ebx * 4]
        jmp Draw_QKM_cc_rtn
    }
}
 
DWORD FuncKeyMappedInfoAddr = (DWORD)&FuncKeyMappedInfoAddr;
DWORD Draw_FKM_cc_rtn = 0x00875B71;
void _declspec(naked) Draw_FKM_cc()
{
    _asm {
        mov al,  [FuncKeyMappedInfo + ecx * 4]
        lea edi, [FuncKeyMappedInfo + ecx * 4 - 0x94] //  00875B9B会用到
        jmp Draw_FKM_cc_rtn
    }
}
 
DWORD Draw_FKM2_cc_rtn = 0x00875BB4;
void _declspec(naked) Draw_FKM2_cc()
{
    _asm {
        mov ecx, eax
        shl eax, 3
        lea ebx, [FuncKeyMappedInfo + eax + ecx * 4 + 8]
        mov ebx, [ebx]
        jmp Draw_FKM2_cc_rtn
    }
}
 
DWORD DrawConsumeItemCooltime_FKM_cc_rtn = 0x0086F707;
void _declspec(naked) DrawConsumeItemCooltime_FKM_cc()
{
    _asm {
        lea ebx, FuncKeyMappedInfo
        jmp DrawConsumeItemCooltime_FKM_cc_rtn
    }
}
 
//DWORD DrawSkillCooltime_FKM_cc_rtn = 0x0086FFF8;
//void _declspec(naked) DrawSkillCooltime_FKM_cc()
//{
//    _asm {
//        lea ecx, [FuncKeyMappedInfo + 1]
//        jmp DrawSkillCooltime_FKM_cc_rtn
//    }
//}
 
DWORD DrawSkillCooltime_SCT_cc_rtn = 0x0086FFF8;
void _declspec(naked) DrawSkillCooltime_SCT_cc()
{
    _asm {
        lea edi, FuncKeyMappedSkillCooltime
        //lea ecx, [esi + 95h]
        lea ecx, [FuncKeyMappedInfo + 1]
        jmp DrawSkillCooltime_SCT_cc_rtn
    }
}
 
void RestoreQKM() {
    memcpy(QuickslotKeyMap, QuickslotKeyMapDefault, sizeof(QuickslotKeyMap));
}
 
DWORD DefaultQuickslotKeyMap_cc_rtn = 0x006C6001;
void _declspec(naked) DefaultQuickslotKeyMap_cc()
{
    _asm {
        call RestoreQKM
        jmp DefaultQuickslotKeyMap_cc_rtn
    }
}
 
DWORD CDraggableMenu_MapFuncKey_QKM_cc_rtn = 0x00509B7E;
void _declspec(naked) CDraggableMenu_MapFuncKey_QKM_cc()
{
    _asm {
        lea eax, [QuickslotKeyMap + ebx * 4]
        mov eax, [eax]
        push eax
        jmp CDraggableMenu_MapFuncKey_QKM_cc_rtn
    }
}
 
DWORD CDraggableSkill_MapFuncKey_QKM_cc_rtn = 0x0050A443;
void _declspec(naked) CDraggableSkill_MapFuncKey_QKM_cc()
{
    _asm {
        lea eax, [QuickslotKeyMap + ebx * 4]
        mov eax, [eax]
        push eax
        jmp CDraggableSkill_MapFuncKey_QKM_cc_rtn
    }
}
 
DWORD CDraggableItem_MapFuncKey_QKM_cc_rtn = 0x005018AB;
void _declspec(naked) CDraggableItem_MapFuncKey_QKM_cc()
{
    _asm {
        lea eax, [QuickslotKeyMap + ebp * 4]
        mov eax, [eax]
        push eax
        jmp CDraggableItem_MapFuncKey_QKM_cc_rtn
    }
}
 
DWORD TryBeginDragFuncKeyMappedIcon_QKM_cc_rtn = 0x00876C94;
void _declspec(naked) TryBeginDragFuncKeyMappedIcon_QKM_cc()
{
    _asm {
        mov edx, QuickslotKeyMapAddr
        mov eax, [edx + eax * 4]
        jmp TryBeginDragFuncKeyMappedIcon_QKM_cc_rtn
    }
}
 
DWORD OnMouseMove_cc_rtn = 0x0087ACCB;
void _declspec(naked) OnMouseMove_cc()
{
    _asm {
        mov eax, edi
        shl eax, 3
        lea edx, [FuncKeyMappedInfo + eax + edi * 4]
        mov al, [edx]
        lea edi, [edx]
        jmp OnMouseMove_cc_rtn
    }
}
 
 
DWORD QuickSlotIsEnableFunc = 0x0086D2F0;
DWORD StatusBarClickRange_Rtn_INT = 0x0086D62F;
DWORD StatusBarClickRange_Rtn_ZERO = 0x0086D626;
DWORD StatusBarClickRange_Rtn;
void StatusBarClickRangeFunc()
{
    int m_nChatWndType; // 聊天窗状态 1=隐藏 3=显示
    int m_nChatWndHeight; // 聊天窗高度 最大值为489
    int rx; // 鼠标位置，从statusbar算起，而非整个游戏窗口
    int ry; // 鼠标位置，从statusbar算起，而非整个游戏窗口
    int m_bDragChatWnd; // 是否拖拽状态
    int quickSlotIsEnabled;
 
    _asm {
        push eax
        mov  eax, [esi + 1804h]
        mov  m_nChatWndType, eax
        mov  eax, [esi + 1800h]
        mov  m_nChatWndHeight, eax
        mov  rx, edi
        mov  ry, ebx
        mov  eax, [esi + 1618h]
        mov  m_bDragChatWnd, eax
        lea  ecx, [esi + 1824h]
        call QuickSlotIsEnableFunc
        mov  quickSlotIsEnabled, eax
        pop  eax
    }
 
    int chatTopY = 489 - m_nChatWndHeight;
 
    StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_ZERO;
    int curWidth = CWvsContext::GetInstance()->m_nScreenWidth;
    if (curWidth > 800)
    {
        if (m_nChatWndType == 1)
        {
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
        }
        else {
            if (rx < 564 && ry > chatTopY) // 鼠标在聊天框头部以下的区域
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            //if (m_bDragChatWnd == 1 && rx < 564 && ry > chatTopY) // 拖拽聊天框状态下鼠标在状态栏
            //{
            //    StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            //}
        }
    }
    else {
        if (m_nChatWndType == 1)
        {
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            if (quickSlotIsEnabled == 1 && rx > 368 && ry > 413) // 鼠标在快捷栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
        }
        else {
            if (rx < 564 && ry > chatTopY) // 鼠标在聊天框头部以下的区域
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            if (quickSlotIsEnabled == 1 && rx > 368 && ry > 413) // 鼠标在快捷栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
        }
    }
 
}
void _declspec(naked) StatusBarClickRange()
{
    _asm {
        call StatusBarClickRangeFunc
        jmp StatusBarClickRange_Rtn
    }
}
 
void AttachQuickSlot() {
    Patch4(0x00876BFE + 1, 1320); // CUIStatusBar宽度扩充1024 -> 1320
    //Patch4(0x0087B62A + 1, 312); // 800x600 quickslot位置 // 会移动整个框架
    PatchJmp(0x00875406, reinterpret_cast<uintptr_t>(&QuickSlotBtOpenCc), 9);
    PatchJmp(0x00875475, reinterpret_cast<uintptr_t>(&QuickSlotBtCloseCc), 9);
    PatchJmp(0x00875102, reinterpret_cast<uintptr_t>(&QuickSlotXcc), 6);
    //Patch4(0x0086D582 + 2, 368);// 可按区域 针对800x600 由下面替代了
    //Patch4(0x0086D5BA + 2, 91);// 可按区域 针对800x600 由下面替代了
    PatchJmp(0x0086D582, reinterpret_cast<uintptr_t>(&StatusBarClickRange), 164);
 
    //CQuickslotKeyMappedMan::SaveQuickslotKeyMap
    PatchJmp(0x006C60D5, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc), 6); // 发送服务端保存
    PatchJmp(0x006C60FC, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc), 10);
 
    //CQuickslotKeyMappedMan::OnInit
    //PatchJmp(0x006C6144, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_OnInit_QKM_cc)); // 接收服务端发来的请求 会重写覆盖本地的数组 // 配合上面的发送请求理论上可以让用户自定义快捷键布局
    PatchJmp(0x006C6169, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_OnInit_Copy_cc), 13);
 
    //CQuickslotKeyMappedMan::IsQuickslotKeyMapModified
    PatchJmp(0x007D8200, reinterpret_cast<uintptr_t>(&IsQuickslotKeyMapModified_QKM_cc), 11);
 
    //CUIKeyConfig::~CUIKeyConfig
    PatchJmp(0x007D8D24, reinterpret_cast<uintptr_t>(&CUIKeyConfig_Copy_cc), 18);
 
    //CUIStatusBar::CQuickSlot::CompareValidateFuncKeyMappedInfo
    PatchJmp(0x008710EB, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_FKM_cc), 5);
    PatchJmp(0x008710F4, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_QKM_cc), 14);
    PatchJmp(0x00871064, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_FKM2_cc), 8);
    Patch4(0x00871112 + 4, 26);
    PatchJmp(0x0087108B, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_cave), 5);
 
    //CUIStatusBar::CQuickSlot::Draw
    PatchJmp(0x008761CA, reinterpret_cast<uintptr_t>(&Draw_QKM_cc), 10);
    PatchJmp(0x00875B67, reinterpret_cast<uintptr_t>(&Draw_FKM_cc), 10);
    PatchJmp(0x00875BAD, reinterpret_cast<uintptr_t>(&Draw_FKM2_cc), 7);
    Patch1(0x0087639B + 2, 26);
    Patch1(0x008762E4 + 2, 4);// 角标x
    Patch1(0x008762DA + 2, 4);// 角标y
 
    //CUIStatusBar::CQuickSlot::GetPosByIndex
    Patch4(0x0086CCA9 + 3, (DWORD)&QuickSlotKeyPos);
    Patch4(0x0086CCB6 + 3, (DWORD)&QuickSlotKeyPos + 4);
    Patch1(0x0086CCA4 + 2, 26);
 
    //CUIStatusBar::GetShortCutIndexByPos
    Patch4(0x0086D06C + 3, (DWORD)&QuickSlotKeyPos);
    Patch4(0x0086D0A7 + 3, (DWORD)&QuickSlotKeyPos + 4);
    Patch4(0x0086D0E2 + 3, (DWORD)&QuickSlotKeyPos);
    Patch4(0x0086D11D + 3, (DWORD)&QuickSlotKeyPos + 4);
    Patch1(0x0086D14A + 2, 26);
 
    //CDraggableMenu::OnDropped
    Patch1(0x00509D71 + 2, 25);
    //CDraggableMenu::MapFuncKey
    Patch1(0x00509AF9 + 2, 26);
    PatchJmp(0x00509B75, reinterpret_cast<uintptr_t>(&CDraggableMenu_MapFuncKey_QKM_cc), 9);
    //CDraggableSkill::MapFuncKey
    PatchJmp(0x0050A43A, reinterpret_cast<uintptr_t>(&CDraggableSkill_MapFuncKey_QKM_cc), 9);
    //CDraggableItem::MapFuncKey
    PatchJmp(0x005018A2, reinterpret_cast<uintptr_t>(&CDraggableItem_MapFuncKey_QKM_cc), 9);
 
    //CUIStatusBar::CQuickSlot::DrawConsumeItemCooltime
    PatchJmp(0x0086F701, reinterpret_cast<uintptr_t>(&DrawConsumeItemCooltime_FKM_cc), 6);
 
    //CUIStatusBar::CQuickSlot::DrawSkillCooltime
    //PatchJmp(0x0086FFF2, reinterpret_cast<uintptr_t>(&DrawSkillCooltime_FKM_cc), 6); // 下面已覆盖
    PatchJmp(0x0086FFEF, reinterpret_cast<uintptr_t>(&DrawSkillCooltime_SCT_cc), 9);
    Patch1(0x0087035E + 2, 26);
 
    //CQuickslotKeyMappedMan::DefaultQuickslotKeyMap
    PatchJmp(0x006C5FF2, reinterpret_cast<uintptr_t>(&DefaultQuickslotKeyMap_cc), 15);
 
    //CUIStatusBar::TryBeginDragFuncKeyMappedIcon
    PatchJmp(0x00876C8A, reinterpret_cast<uintptr_t>(&TryBeginDragFuncKeyMappedIcon_QKM_cc), 10);
 
    //CUIStatusBar::OnMouseMove 修复tooltip
    PatchJmp(0x0087ACBE, reinterpret_cast<uintptr_t>(&OnMouseMove_cc), 13);
 
    //CQuickSlot::~CQuickSlot
    PatchJmp(0x0086FB19, reinterpret_cast<uintptr_t>(&ResetFKMInfo), 6);
}