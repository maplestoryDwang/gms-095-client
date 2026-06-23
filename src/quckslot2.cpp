#include "hook.h"
#include "wvs/wvscontext.h"


// 26键位 一排13个
// QuickslotKeyMapDefault、QuickslotKeyMap 和 QuickslotKeyMapOld 存放的是按键数据，每行第一个Byte是键码，
// 这个键码你可以在 UI.wz/UIWindow.img/KeyConfig/key 中找到，请保持三个数组一致。如果要扩展的话，除了添加行数以外还要修改后面的代码。
UCHAR QuickslotKeyMapDefault[] = {
        42, 0, 0, 0,
        82, 0, 0, 0,
        71, 0, 0, 0,
        73, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        4, 0, 0, 0,
        5, 0, 0, 0,
        6, 0, 0, 0,
        7, 0, 0, 0,
        8, 0, 0, 0,
        9, 0, 0, 0,
        10, 0, 0, 0,
 
        29, 0, 0, 0,
        83, 0, 0, 0,
        79, 0, 0, 0,
        81, 0, 0, 0,
        16, 0, 0, 0,
        17, 0, 0, 0,
        18, 0, 0, 0,
        19, 0, 0, 0,
        20, 0, 0, 0,
        30, 0, 0, 0,
        31, 0, 0, 0,
        32, 0, 0, 0,
        33, 0, 0, 0,
};
 
// 26x4 = 104
UCHAR QuickslotKeyMap[] = {
        42, 0, 0, 0,
        82, 0, 0, 0,
        71, 0, 0, 0,
        73, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        4, 0, 0, 0,
        5, 0, 0, 0,
        6, 0, 0, 0,
        7, 0, 0, 0,
        8, 0, 0, 0,
        9, 0, 0, 0,
        10, 0, 0, 0,
 
        29, 0, 0, 0,
        83, 0, 0, 0,
        79, 0, 0, 0,
        81, 0, 0, 0,
        16, 0, 0, 0,
        17, 0, 0, 0,
        18, 0, 0, 0,
        19, 0, 0, 0,
        20, 0, 0, 0,
        30, 0, 0, 0,
        31, 0, 0, 0,
        32, 0, 0, 0,
        33, 0, 0, 0,
};
 
UCHAR QuickslotKeyMapOld[] = {
        42, 0, 0, 0,
        82, 0, 0, 0,
        71, 0, 0, 0,
        73, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        4, 0, 0, 0,
        5, 0, 0, 0,
        6, 0, 0, 0,
        7, 0, 0, 0,
        8, 0, 0, 0,
        9, 0, 0, 0,
        10, 0, 0, 0,
 
        29, 0, 0, 0,
        83, 0, 0, 0,
        79, 0, 0, 0,
        81, 0, 0, 0,
        16, 0, 0, 0,
        17, 0, 0, 0,
        18, 0, 0, 0,
        19, 0, 0, 0,
        20, 0, 0, 0,
        30, 0, 0, 0,
        31, 0, 0, 0,
        32, 0, 0, 0,
        33, 0, 0, 0,
};
 

// QuickSlotKeyPos 存放的是每个按键的x/y坐标，分别对应每行第 1 列和第 5 列，行数和上面的三个数组对应，083的长键盘同理
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
        147, 1, 0, 0, 15, 0, 0, 0, //x坐标也是+33 ，  y坐标 + 33

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
 
// FuncKeyMappedInfo 和 FuncKeyMappedSkillCooltime是存放临时数据的，如果你要扩展更多按键的话，别忘了扩展这两个数组。

// 312/26 = 一个键位12个0
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
 
// 104/26 = 4  每个slot4个0
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
 
// 312 / 26 = 12（）
void resetFuncKeyMappedInfo() {
    for (int i = 0; i < 312; ++i) {
        FuncKeyMappedInfo[i] = 0;
    }
}
 
 /*
     .text:0086FB19 8B 86 8C 00 00 00                 mov     eax, [esi+8Ch]
     .text:0086FB1F C7 44 24 18 05 00                 mov     [esp+1Ch+var_4], 5
*/
DWORD ResetFKMInfo_Retn = 0x0086FB1F;
// _declspec(naked) 的意思就是：“这是一个‘裸函数’，编译器你什么都别管，不要自作聪明给我加任何多余的堆栈初始化或返回代码，里面写了什么汇编，生成出来的就是什么
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
 
 /*

    .text:0087108B 6A 60                             push    60h ; '`'       ; count
    .text:0087108D 6A 00                             push    0               ; value
    .text:0087108F 53                                push    ebx             ; dst
    .text:00871090 E8 EB F1 1B 00                    call    _memset
 */
DWORD CompareValidate_Hook_start = 0x0087108B;
DWORD CompareValidate_Retn = 0x00871090;
_declspec(naked) void CompareValidateFuncKeyMappedInfo_cave()
{
    _asm
    {
        push 0x138   // 这里增加了  312
        push 0x0
        push ebx
        jmp CompareValidate_Retn
    }
}
 
 // 800x600的那两个按钮值
DWORD QuickSlotBtOpenCcRtn = 0x0087540F;
_declspec(naked) void QuickSlotBtOpenCc()
{
    _asm {
        push 0    // y 偏移
        push 40  // x偏移 原来80
        push 1006 // id
        jmp QuickSlotBtOpenCcRtn
    }
}
 
DWORD QuickSlotBtCloseCcRtn = 0x0087547E;
_declspec(naked) void QuickSlotBtCloseCc()
{
    _asm {
        push 0    // y 偏移
        push 40  // x偏移 原来80
        push 1006 // id
        jmp QuickSlotBtCloseCcRtn
    }
}
 
/*
.text:00875102 8B 4D E8                          mov     ecx, [ebp+i]
.text:00875105 89 4E 08                          mov     [esi+8], ecx
.text:00875108 8B 7E 10                          mov     edi, [esi+10h]
*/
DWORD QuickSlotXccRtn = 0x00875108;
_declspec(naked) void QuickSlotXcc()
{
    _asm {
        mov ecx, 358 // 800-442
        mov[esi + 8], ecx
        jmp QuickSlotXccRtn
    }
}
 
 
/*
.text:006C60D5 6A 20                             push    20h ; ' '       ; uSize
.text:006C60D7 8D 77 04                          lea     esi, [edi+4]
.text:006C60DA 56                                push    esi             ; p
.text:006C60DB 8D 4C 24 14                       lea     this, [esp+30h+oPacket] ; this
*/
DWORD QuickslotKeyMapAddr = (DWORD)&QuickslotKeyMap;

// 发给服务端保存的
DWORD CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc_hook = 0x006C60D5;
DWORD CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc_rtn = 0x006C60DB;
void _declspec(naked) CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc()
{
    _asm {
        push 32 // 看情况是不是要改成 104，可能要配合服务端  26X4 = 104
        push QuickslotKeyMapAddr
        jmp CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc_rtn
    }
}
 

/*
.text:006C60FC 83 C7 24                          add     edi, 24h ; '$'
.text:006C60FF B9 08 00 00 00                    mov     ecx, 8
.text:006C6104 F3 A5                             rep movsd
.text:006C6106 8D 4C 24 10                       lea     this, [esp+28h+oPacket.m_aSendBuff] ; this

movsd (Move String Doubleword)：
它的功能是：把 esi 寄存器 指向的内存地址里的一个“双字”（Doubleword，即 4 个字节），复制到 edi 寄存器 指向的内存地址。
复制完成后，esi 和 edi 会自动 +4（指向下一个 4 字节的位置）。
rep 前缀：
它的功能是：重复执行后面的指令，重复的次数由 ecx 寄存器决定。
每次重复，ecx 就会 -1，直到 ecx 变为 0 循环结束。

*/
// keyMap 复制到keyMapOld
void copyQKMtoOld() {
    memcpy(QuickslotKeyMapOld, QuickslotKeyMap, sizeof(QuickslotKeyMapOld));
}
 
DWORD CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc_hook = 0x006C60FC;
DWORD CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc_rtn = 0x006C6106;
void _declspec(naked) CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc()
{
    _asm {
        call copyQKMtoOld
        jmp CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc_rtn
    }
}
 
// 暂时没用上 不使用默认布局
DWORD CQuickslotKeyMappedMan_OnInit_QKM_cc_hook = 0x006C6144;
DWORD CQuickslotKeyMappedMan_OnInit_QKM_cc_rtn = 0x006C614A;
void _declspec(naked) CQuickslotKeyMappedMan_OnInit_QKM_cc()
{
    _asm {
        push 32 // 看情况是不是要改成 104，可能要配合服务端
        push QuickslotKeyMapAddr
        jmp CQuickslotKeyMappedMan_OnInit_QKM_cc_rtn
    }
}
 
/*
.text:006C6169 8D 73 04                          lea     esi, [ebx+4]
.text:006C616C 8D 7B 24                          lea     edi, [ebx+24h]
.text:006C616F B9 08 00 00 00                    mov     this, 8
.text:006C6174 F3 A5                             rep movsd
.text:006C6176 8B CB                             mov     this, ebx       ; this
*/
DWORD CQuickslotKeyMappedMan_OnInit_Copy_cc_hook = 0x006C6169;
DWORD CQuickslotKeyMappedMan_OnInit_Copy_cc_rtn = 0x006C6176;
void _declspec(naked) CQuickslotKeyMappedMan_OnInit_Copy_cc()
{
    _asm {
        call copyQKMtoOld
        jmp CQuickslotKeyMappedMan_OnInit_Copy_cc_rtn
    }
}
 
/*
.text:007D8200 8D 51 04                          lea     edx, [this+4]
.text:007D8203 B8 20 00 00 00                    mov     eax, 20h ; ' '
.text:007D8208 83 C1 24                          add     this, 24h ; '$'
.text:007D820B 56                                push    esi
*/
DWORD QuickslotKeyMapOldAddr = (DWORD)&QuickslotKeyMapOld;
DWORD IsQuickslotKeyMapModified_QKM_cc_hook = 0x007D8200;
DWORD IsQuickslotKeyMapModified_QKM_cc_rtn = 0x007D820B;
void _declspec(naked) IsQuickslotKeyMapModified_QKM_cc()         //判断是否修改
{
    _asm {
        mov edx, QuickslotKeyMapAddr
        mov eax, 104                        // 20h = 32 -》104 = 26X4
        mov ecx, QuickslotKeyMapOldAddr
        jmp IsQuickslotKeyMapModified_QKM_cc_rtn
    }
}
 

/*
.text:007D8D24 A1 E8 AB C6 00                    mov     eax, ?ms_pInstance@?$TSingleton@VCQuickslotKeyMappedMan@@@@1PAVCQuickslotKeyMappedMan@@A ; CQuickslotKeyMappedMan * TSingleton<CQuickslotKeyMappedMan>::ms_pInstance
.text:007D8D29 8D 70 24                          lea     esi, [eax+24h]
.text:007D8D2C 8D 78 04                          lea     edi, [eax+4]
.text:007D8D2F B9 08 00 00 00                    mov     this, 8
.text:007D8D34 F3 A5                             rep movsd
.text:007D8D36 8B 0D C8 8A C6 00                 mov     this, ?ms_pInstance@?$TSingleton@VCUIStatusBar@@@@1PAVCUIStatusBar@@A._m_pStr ; this                          rep movsd
*/
// 旧的复制给新的
void copyOldtoQKM() {
    memcpy(QuickslotKeyMap, QuickslotKeyMapOld, sizeof(QuickslotKeyMap));
}
/*
  qmemcpy(
    TSingleton<CQuickslotKeyMappedMan>::ms_pInstance->m_aQuickslotKeyMapped,
    TSingleton<CQuickslotKeyMappedMan>::ms_pInstance->m_aQuickslotKeyMapped_Old,
    sizeof(TSingleton<CQuickslotKeyMappedMan>::ms_pInstance->m_aQuickslotKeyMapped));
*/
DWORD CUIKeyConfig_Copy_cc_hook = 0x007D8D24;
DWORD CUIKeyConfig_Copy_cc_rtn = 0x007D8D36;
void _declspec(naked) CUIKeyConfig_Copy_cc()
{
    _asm {
        call copyOldtoQKM
        jmp  CUIKeyConfig_Copy_cc_rtn
    }
}
 

/*
.text:008710EB A1 E4 AA C6 00                    mov     eax, ?ms_pInstance@?$TSingleton@VCFuncKeyMappedMan@@@@1PAVCFuncKeyMappedMan@@A ; CFuncKeyMappedMan * TSingleton<CFuncKeyMappedMan>::ms_pInstance
.text:008710F0 89 44 24 28                       mov     [esp+40h+pFkmMan], eax
*/
DWORD CompareValidateFuncKeyMappedInfo_FKM_cc_hook = 0x008710EB;
DWORD CompareValidateFuncKeyMappedInfo_FKM_cc_rtn = 0x008710F0;
void _declspec(naked) CompareValidateFuncKeyMappedInfo_FKM_cc()
{
    _asm {
        lea ebx, FuncKeyMappedInfo  //自己的放到EBX？
        mov eax, [0x00C6AAE4]   //.data:00C6AAE4 ?? ?? ?? ??       ?ms_pInstance@?$TSingleton@VCFuncKeyMappedMan@@@@1PAVCFuncKeyMappedMan@@A dd ?
        mov eax, [eax]
        jmp CompareValidateFuncKeyMappedInfo_FKM_cc_rtn
    }
}
 

/*
.text:008710F4 A1 E8 AB C6 00                    mov     eax, ?ms_pInstance@?$TSingleton@VCQuickslotKeyMappedMan@@@@1PAVCQuickslotKeyMappedMan@@A ; CQuickslotKeyMappedMan * TSingleton<CQuickslotKeyMappedMan>::ms_pInstance
.text:008710F9 81 C3 94 00 00 00                 add     ebx, 94h
.text:008710FF 83 C0 04                          add     eax, 4
.text:00871102 C7 44 24 10 01 00                 mov     [esp+40h+bSame], 1

*/
DWORD CompareValidateFuncKeyMappedInfo_QKM_cc_hook = 0x008710F4;
DWORD CompareValidateFuncKeyMappedInfo_QKM_cc_rtn = 0x00871102;
void _declspec(naked) CompareValidateFuncKeyMappedInfo_QKM_cc()
{
    _asm {
        mov eax, QuickslotKeyMapAddr
        //add ebx, 0x94
        jmp CompareValidateFuncKeyMappedInfo_QKM_cc_rtn
    }
}
 
/*
.text:00871064 81 C3 94 00 00 00                 add     ebx, 94h
.text:0087106A 8B CB                             mov     ecx, ebx
.text:0087106C 8D 64 24 00                       lea     esp, [esp+0]
v6 = this->m_aFuncKeyMappedInfo;
*/
DWORD CompareValidateFuncKeyMappedInfo_FKM2_cc_hook = 0x00871064;
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
DWORD StatusBarClickRange_Rtn;  // 返回值

void StatusBarClickRangeFunc() {
    int m_nChatWndType;   // 聊天窗状态 1=隐藏 3=显示
    int m_nChatWndHeight; // 聊天窗高度 最大值为489
    int rx;               // 鼠标位置，从statusbar算起，而非整个游戏窗口
    int ry;               // 鼠标位置，从statusbar算起，而非整个游戏窗口
    int m_bDragChatWnd;   // 是否拖拽状态
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

    StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_ZERO; //不可点击
    int curWidth = CWvsContext::GetInstance()->m_nScreenWidth;

    if (curWidth > 800) {
        if (m_nChatWndType == 1) {
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
        } else {
            if (rx < 564 && ry > chatTopY) // 鼠标在聊天框头部以下的区域
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            // if (m_bDragChatWnd == 1 && rx < 564 && ry > chatTopY) // 拖拽聊天框状态下鼠标在状态栏
            //{
            //     StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            // }
        }
    } else {
        if (m_nChatWndType == 1) {
            if (rx < 1321 && ry > 506) // 鼠标在状态栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
            if (quickSlotIsEnabled == 1 && rx > 368 && ry > 413) // 鼠标在快捷栏
            {
                StatusBarClickRange_Rtn = StatusBarClickRange_Rtn_INT;
            }
        } else {
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


//StatusBarClickRangeFunc 这个方法很重要，它会影响你鼠标的可点击区域。因为官方的客户端逻辑很乱，所以我这里直接 Hook 出来重写了。
void _declspec(naked) StatusBarClickRange()
{
    _asm {
        call StatusBarClickRangeFunc
        jmp StatusBarClickRange_Rtn
    }
}
 
void AttachQuickSlot() {
    
    // ----------------------------------------------------------------------
    //  void __thiscall CUIStatusBar::CUIStatusBar(CUIStatusBar *this)
    // ----------------------------------------------------------------------
    Patch4(0x00876BFE + 1, 1320); // CUIStatusBar宽度扩充1024 -> 1320
    // Patch4(0x0087B62A + 1, 312); // 800x600 quickslot位置 // 会移动整个框架

    // ----------------------------------------------------------------------
    // CUIStatusBar::CQuickSlot::Init
    // 修改按钮的未知
    // ----------------------------------------------------------------------
    PatchJmp(0x00875406, reinterpret_cast<uintptr_t>(&QuickSlotBtOpenCc), 9);  // CUIStatusBar::CQuickSlot::Init
    PatchJmp(0x00875475, reinterpret_cast<uintptr_t>(&QuickSlotBtCloseCc), 9); // CUIStatusBar::CQuickSlot::Init
    PatchJmp(0x00875102, reinterpret_cast<uintptr_t>(&QuickSlotXcc), 6);       // CUIStatusBar::CQuickSlot::Init


    // ----------------------------------------------------------------------
    // CUIStatusBar::HitTest
    // ----------------------------------------------------------------------
    // Patch4(0x0086D582 + 2, 368);// 可按区域 针对800x600 由下面替代了
    // Patch4(0x0086D5BA + 2, 91);// 可按区域 针对800x600 由下面替代了
    // 重构了hittest
    PatchJmp(0x0086D582, reinterpret_cast<uintptr_t>(&StatusBarClickRange), 164);  // CUIStatusBar::HitTest 重构了


    // ----------------------------------------------------------------------
    // CQuickslotKeyMappedMan::SaveQuickslotKeyMap
    // ----------------------------------------------------------------------
    PatchJmp(0x006C60D5, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_SaveQuickslotKeyMap_QKM_cc), 6);     // 复制到要发送的包发送服务端保存
    PatchJmp(0x006C60FC, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_SaveQuickslotKeyMap_Copy_cc), 10);   // 复制新的到旧的数组里面

    // ----------------------------------------------------------------------
    // CQuickslotKeyMappedMan::OnInit
    // ----------------------------------------------------------------------
    // 服务端发的true,不会走0x006C6144
    // PatchJmp(0x006C6144, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_OnInit_QKM_cc)); // 接收服务端发来的请求 会重写覆盖本地的数组 // 配合上面的发送请求理论上可以让用户自定义快捷键布局
    // 和发给服务端一样也是QuickslotKeyMapper 复制到 oldQuickslotKeyMapper
    PatchJmp(0x006C6169, reinterpret_cast<uintptr_t>(&CQuickslotKeyMappedMan_OnInit_Copy_cc), 13);


    // CQuickslotKeyMappedMan::IsQuickslotKeyMapModified 是否修改
    PatchJmp(0x007D8200, reinterpret_cast<uintptr_t>(&IsQuickslotKeyMapModified_QKM_cc), 11);

    // CUIKeyConfig::~CUIKeyConfig
    // 旧的复制给新的
    PatchJmp(0x007D8D24, reinterpret_cast<uintptr_t>(&CUIKeyConfig_Copy_cc), 18);

    // CUIStatusBar::CQuickSlot::CompareValidateFuncKeyMappedInfo
    // 修改对象  把地址写我们自己的对象
    PatchJmp(0x008710EB, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_FKM_cc), 5);
    PatchJmp(0x008710F4, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_QKM_cc), 14);
    PatchJmp(0x00871064, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_FKM2_cc), 8);
    Patch4(0x00871112 + 4, 26);
    PatchJmp(CompareValidate_Hook_start, reinterpret_cast<uintptr_t>(&CompareValidateFuncKeyMappedInfo_cave), 5);

    // CUIStatusBar::CQuickSlot::Draw
    PatchJmp(0x008761CA, reinterpret_cast<uintptr_t>(&Draw_QKM_cc), 10);
    PatchJmp(0x00875B67, reinterpret_cast<uintptr_t>(&Draw_FKM_cc), 10);
    PatchJmp(0x00875BAD, reinterpret_cast<uintptr_t>(&Draw_FKM2_cc), 7);
    Patch1(0x0087639B + 2, 26);         //  8-> 26
    Patch1(0x008762E4 + 2, 4); // 角标x
    Patch1(0x008762DA + 2, 4); // 角标y

    // CUIStatusBar::CQuickSlot::GetPosByIndex
    Patch4(0x0086CCA9 + 3, (DWORD)&QuickSlotKeyPos);
    Patch4(0x0086CCB6 + 3, (DWORD)&QuickSlotKeyPos + 4);
    Patch1(0x0086CCA4 + 2, 26);

    // CUIStatusBar::GetShortCutIndexByPos
    Patch4(0x0086D06C + 3, (DWORD)&QuickSlotKeyPos);
    Patch4(0x0086D0A7 + 3, (DWORD)&QuickSlotKeyPos + 4);
    Patch4(0x0086D0E2 + 3, (DWORD)&QuickSlotKeyPos);
    Patch4(0x0086D11D + 3, (DWORD)&QuickSlotKeyPos + 4);
    Patch1(0x0086D14A + 2, 26);

    // CDraggableMenu::OnDropped
    Patch1(0x00509D71 + 2, 25);      // ShortCutIndexByPos <= 7 

    // CDraggableMenu::MapFuncKey
    Patch1(0x00509AF9 + 2, 26);
    PatchJmp(0x00509B75, reinterpret_cast<uintptr_t>(&CDraggableMenu_MapFuncKey_QKM_cc), 9);
    // CDraggableSkill::MapFuncKey
    PatchJmp(0x0050A43A, reinterpret_cast<uintptr_t>(&CDraggableSkill_MapFuncKey_QKM_cc), 9);
    // CDraggableItem::MapFuncKey
    PatchJmp(0x005018A2, reinterpret_cast<uintptr_t>(&CDraggableItem_MapFuncKey_QKM_cc), 9);

    // CUIStatusBar::CQuickSlot::DrawConsumeItemCooltime
    PatchJmp(0x0086F701, reinterpret_cast<uintptr_t>(&DrawConsumeItemCooltime_FKM_cc), 6);

    // CUIStatusBar::CQuickSlot::DrawSkillCooltime
    // PatchJmp(0x0086FFF2, reinterpret_cast<uintptr_t>(&DrawSkillCooltime_FKM_cc), 6); // 下面已覆盖
    PatchJmp(0x0086FFEF, reinterpret_cast<uintptr_t>(&DrawSkillCooltime_SCT_cc), 9);
    Patch1(0x0087035E + 2, 26);

    // CQuickslotKeyMappedMan::DefaultQuickslotKeyMap
    PatchJmp(0x006C5FF2, reinterpret_cast<uintptr_t>(&DefaultQuickslotKeyMap_cc), 15);

    // CUIStatusBar::TryBeginDragFuncKeyMappedIcon
    PatchJmp(0x00876C8A, reinterpret_cast<uintptr_t>(&TryBeginDragFuncKeyMappedIcon_QKM_cc), 10);

    // CUIStatusBar::OnMouseMove 修复tooltip
    PatchJmp(0x0087ACBE, reinterpret_cast<uintptr_t>(&OnMouseMove_cc), 13);

    // CQuickSlot::~CQuickSlot
    // 汇编代码（ResetFKMInfo）并没有塞进游戏的那个 6 字节漏洞里。它存放在内存中的另一个全新地方。那 6 个字节里只放了一条去往那个新地方的“路标”（jmp 指令）。
    // 在 x86 汇编中，一条标准的长跳转指令 jmp <相对地址> 的机器码是 E9 XX XX XX XX，它固定占用 5 个字节。
    PatchJmp(0x0086FB19, reinterpret_cast<uintptr_t>(&ResetFKMInfo), 6);
}