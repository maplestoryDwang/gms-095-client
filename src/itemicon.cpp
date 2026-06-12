#include "pch.h"
#include "hook.h"
#include "debug.h"
#include "common/dbbasic.h"
#include "common/iteminfo.h"
#include "wvs/userlocal.h"
#include "wvs/util.h"
#include "ztl/ztl.h"
#include <memory>


/*
具体来说，它通过 Hook 游戏原本绘制物品图标的函数，在绘制宠物图标时做了两件事：
如果该宠物是当前召唤出来的宠物，就在它的图标背后画一个淡蓝灰色的背景框。
如果是玩家召唤的第一只宠物（主宠物），则在图标上方额外绘制一个“首领宠物（bossPetIcon）”的特殊小标记。


*/

struct GW_ItemSlotPet : GW_ItemSlotBase {
};


/*
作用：获取玩家当前召唤的宠物数据。
原理：通过 __thiscall 调用游戏内存地址 0x008E3030 处的原生函数。输入参数 nIndex（冒险岛支持同时召唤 3 只宠物，索引为 0, 1, 2），返回对应槽位上活跃宠物的指针。
*/

// CUser::GetActivePetItemSlot 0x008E3030

ZRef<GW_ItemSlotPet> GetActivePetItemSlot(CUser* pUser, int nIndex) {
    ZRef<GW_ItemSlotPet> pItemSlot;
    reinterpret_cast<ZRef<GW_ItemSlotPet>*(__thiscall*)(CUser*, ZRef<GW_ItemSlotPet>*, int)>(0x008E3030)(pUser, std::addressof(pItemSlot), nIndex);
    return pItemSlot;
}

// __fastcall，前两个参数通过寄存器 ECX (pThis) 和 EDX 传递
void __fastcall CItemInfo__DrawItemIconForSlot_helper(CItemInfo* pThis, void* _EDX, GW_ItemSlotBase* pItem, IWzCanvasPtr pCanvas, int nItemID, int x, int y, int bProtectedItem, int bMag2, int bPetDead, int bHideCashIcon, int nEquipItemQuality, int bHideQualityIcon, int nMagSize) {
    int nPetIndex = -1;
    if (pItem->GetType() == 3) {
        for (int i = 0; i < 3; ++i) {
            // 遍历获取本地玩家当前召唤的 3 个宠物
            auto pPetItemSlot = GetActivePetItemSlot(CUserLocal::GetInstance(), i);
            // 如果当前要绘制的物品，就是正在召唤的某只宠物
            if (pPetItemSlot == pItem) {
                nPetIndex = i;
                break;
            }
        }
        // 操作 1：如果是已召唤的宠物，在图标位置画一个矩形背景框

        if (nPetIndex >= 0) {
            pCanvas->DrawRectangle(x + 1, y - 31, 31, 31, 0xFFBBCCDD);
        }
    }
    // 调用游戏原生的图标绘制函数，把宠物原本的宠物狗、小猫等小图标画上去
    // 0x005C0A40  CItemInfo::DrawItemIconForSlot
    pThis->DrawItemIconForSlot(pCanvas, nItemID, x, y, bProtectedItem, bMag2, bPetDead, bHideCashIcon, nEquipItemQuality, bHideCashIcon, nMagSize);
    // 操作 2：如果是第一只宠物（主宠物），额外堆叠一个“bossPetIcon”图标
    if (nPetIndex == 0) {
        // 从游戏的 UIWindow.img 资源包中读取名为 "bossPetIcon" 的图片
        IWzCanvasPtr pBossPetIcon = get_unknown(get_rm()->GetObjectA(L"UI/UIWindow.img/Item/bossPetIcon"));
        // 使用 CopyEx 将这个小图标覆盖绘制到宠物图标的左上角 (x-1, y-37) 处
        pCanvas->CopyEx(x - 1, y - 37, pBossPetIcon, CANVAS_ALPHATYPE::CA_REMOVEALPHA, 0, 0, 0, 0, 0, 0);
    }
}

/*
这是一个汇编级别的 Hook 桥梁。当游戏执行到 0x007CD3FB（即 CUIItem::Draw 内部绘制图标的地方）时，会触发跳转来到这里。
这里将寄存器 ESI（当前物品指针）压栈作为参数，然后调用我们写好的 helper 函数，最后用 jmp 安全地返回到游戏原代码地址 0x007CD400 继续执行。
*/
static auto CItemInfo__DrawItemIconForSlot_jmp = 0x007CD3FB;
static auto CItemInfo__DrawItemIconForSlot_ret = 0x007CD400;
void __declspec(naked) CItemInfo__DrawItemIconForSlot_hook() {
    __asm {
        push    esi ; GW_ItemSlotBase*                  // 此时 ESI 寄存器里刚好存着游戏原本的物品指针 (GW_ItemSlotBase*)
        call    CItemInfo__DrawItemIconForSlot_helper   // 注意是__fastcall
        jmp     [ CItemInfo__DrawItemIconForSlot_ret ]
    }
}

void AttachIconIconMod() {

    // 在游戏原本绘制图标的地址（0x007CD3FB）写入一个 JMP 跳转到我们的 hook 函数
    PatchJmp(CItemInfo__DrawItemIconForSlot_jmp, reinterpret_cast<uintptr_t>(&CItemInfo__DrawItemIconForSlot_hook)); // CUIItem::Draw
}
