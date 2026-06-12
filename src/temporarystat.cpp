#include "pch.h"
#include "hook.h"
#include "ztl/ztl.h"
#include "wvs/util.h"
#include "wvs/wvsapp.h"
#include "wvs/wvscontext.h"
#include "wvs/temporarystatview.h"
#include <timeapi.h>
#include <cstdio>
#include <cstdint>

#define COOLTIME_OFFSET_HEIGHT 40

/*
它的核心目的是：通过复用游戏原生的右上角“临时状态栏”（Buff 图标栏），
在它的下方额外渲染一套“技能冷却时间（Cooltime）”的图标，并在图标上实时绘制倒计时数字。
*/



static int g_tLastUpdate = -1;

// 具体秒显示
static IWzPropertyPtr g_pPropSecond;

// CD显示的CTemporaryStatView
static CTemporaryStatView g_tsvCooltime;

static auto CTemporaryStatView__Clear = reinterpret_cast<void(__thiscall*)(CTemporaryStatView*)>(0x0075C330);

void __fastcall CTemporaryStatView__Clear_hook(CTemporaryStatView* pThis, void* _EDX) {
    CTemporaryStatView__Clear(pThis);
    CTemporaryStatView__Clear(&g_tsvCooltime);
}

static auto CTemporaryStatView__Show = reinterpret_cast<void(__thiscall*)(CTemporaryStatView*)>(0x0075C6A0);

void __fastcall CTemporaryStatView__Show_hook(CTemporaryStatView* pThis, void* _EDX) {
    CTemporaryStatView__Show(pThis);
    CTemporaryStatView__Show(&g_tsvCooltime);
}

static auto CTemporaryStatView__Hide = reinterpret_cast<void(__thiscall*)(CTemporaryStatView*)>(0x0075C8C0);

void __fastcall CTemporaryStatView__Hide_hook(CTemporaryStatView* pThis, void* _EDX) {
    CTemporaryStatView__Hide(pThis);
    CTemporaryStatView__Hide(&g_tsvCooltime);
}

static auto CTemporaryStatView__Update = reinterpret_cast<void(__thiscall*)(CTemporaryStatView*)>(0x0075DC50);

void __fastcall CTemporaryStatView__Update_hook(CTemporaryStatView* pThis, void* _EDX) {
    CTemporaryStatView__Update(pThis);
    CTemporaryStatView__Update(&g_tsvCooltime);
    g_tLastUpdate = CWvsApp::GetInstance()->m_tUpdateTime;
}

// CTemporaryStatView::AdjustPosition
static auto CTemporaryStatView__AdjustPosition = reinterpret_cast<void(__thiscall*)(CTemporaryStatView*)>(0x0075CAD0);

void AdjustPositionWithOffset(CTemporaryStatView* tsv, int32_t nOffset) {
    CTemporaryStatView__AdjustPosition(tsv);
    auto pos = tsv->m_lTemporaryStat.GetHeadPosition();
    while (pos) {
        // ... 遍历所有图标，将它们的 pLayer 和 pLayerShadow 向下平移 nOffset 像素
        auto pNext = ZList<ZRef<CTemporaryStatView::TEMPORARY_STAT>>::GetNext(pos);
        pNext->pLayer->RelOffset(0, nOffset);
        pNext->pLayerShadow->RelOffset(0, nOffset);
    }
}

void __fastcall CTemporaryStatView__AdjustPosition_hook(CTemporaryStatView* pThis, void* _EDX) {
    CTemporaryStatView__AdjustPosition(pThis);                        // 计算原 Buff 栏位置
    AdjustPositionWithOffset(&g_tsvCooltime, COOLTIME_OFFSET_HEIGHT); // 将冷却栏向下移动 40 像
}

// 原来只在quickslot显示的CD现在在右上角也能显示。
static auto CWvsContext__SetSkillCooltimeOver = reinterpret_cast<void(__thiscall*)(CWvsContext*, int32_t, int32_t)>(0x009DB0D0);

void __fastcall CWvsContext__SetSkillCooltimeOver_hook(CWvsContext* pThis, void* _EDX, int32_t nSkillID, int32_t tTimeOver) {
    CWvsContext__SetSkillCooltimeOver(pThis, nSkillID, tTimeOver);
    // Battleship durability
    if (nSkillID == 5221999) {
        return;
    }
    // 如果该技能已经在冷却列表中，更新剩余时间
    // 如果不在，计算一个 Flag（通过位运算防止冲突），并调用 SetTemporary 将其加入 g_tsvCooltime
    // 作用：当服务器发来“技能进入冷却”的封包时，游戏会调用 SetSkillCooltimeOver。
    // Hook 拦截后，计算出剩余时间（tTimeOver - timeGetTime()），然后强行把这个技能作为“临时状态（Buff）”塞进 g_tsvCooltime 队列里。
    // 这样，技能图标就能像 Buff 一样在右上角显示了。
    // 相当于CD也是一个技能的BUFF
    int32_t nRemain = tTimeOver - timeGetTime();
    auto pos = g_tsvCooltime.m_lTemporaryStat.GetHeadPosition();
    while (pos) {
        auto pNext = ZList<ZRef<CTemporaryStatView::TEMPORARY_STAT>>::GetNext(pos);
        if (pNext->nID == nSkillID) {
            pNext->tLeft = nRemain;
            return;
        }
    }
    UINT128 uFlag;
    uFlag.setBitNumber((nSkillID % 127) + 1, 1); // hope for no collisions
    g_tsvCooltime.SetTemporary(TSV_SKILL, nSkillID, nRemain, uFlag, ZXString<char>(), 0, 0);
    AdjustPositionWithOffset(&g_tsvCooltime, COOLTIME_OFFSET_HEIGHT);
}

static auto CWvsContext__RemoveSkillCooltimeReset = reinterpret_cast<void(__thiscall*)(CWvsContext*, int32_t)>(0x009CCF80);

void __fastcall CWvsContext__RemoveSkillCooltimeReset_hook(CWvsContext* pThis, void* _EDX, int32_t nSkillID) {
    CWvsContext__RemoveSkillCooltimeReset(pThis, nSkillID);
    g_tsvCooltime.ResetTemporary(TSV_SKILL, nSkillID);
}

// CTemporaryStatView::TEMPORARY_STAT::UpdateShadowIndex
static auto TEMPORARY_STAT__UpdateShadowIndex = 0x0075D560;

void __fastcall TEMPORARY_STAT__UpdateShadowIndex_hook(CTemporaryStatView::TEMPORARY_STAT* pThis, void* _EDX) {
    if (pThis->bNoShadow) {
        return;
    }
    int32_t nSeconds = pThis->tLeft / 1000; // 计算剩余秒数
    if (nSeconds == pThis->nIndexShadow) {
        return;
    }
    pThis->nIndexShadow = nSeconds; // hijack nIndexShadow to redraw every second
    int32_t nShadowIndex = 0;
    if (pThis->tLeftUnit) {
        nShadowIndex = pThis->tLeft / pThis->tLeftUnit;
        if (nShadowIndex < 0) {
            nShadowIndex = 0;
        } else if (nShadowIndex > 15) {
            nShadowIndex = 15;
        }
    }

// 1. 移除旧的阴影画布
   pThis->pLayerShadow->RemoveCanvas(-2);


    // resolve shadow canvas// 2. 加载游戏原生冷却阴影图片 (UI/UIWindow.img/Skill/CoolTime/...)
    wchar_t sShadowProperty[256];
    swprintf_s(sShadowProperty, 256, L"UI/UIWindow.img/Skill/CoolTime/%d", nShadowIndex);
    IWzCanvasPtr pShadowCanvas = get_unknown(get_rm()->GetObjectA(Ztl_bstr_t(sShadowProperty)));

    // create copy of shadow canvas// 3. 创建一个新的 32x32 画布，把阴影复制进去
    IWzCanvasPtr pNewCanvas;
    PcCreateObject<IWzCanvasPtr>(L"Canvas", pNewCanvas, nullptr);
    pNewCanvas->Create(32, 32);
    pNewCanvas->Copy(0, 0, pShadowCanvas);

    // draw number on canvas // 数字描绘
    if (!g_pPropSecond) {
        g_pPropSecond = get_unknown(get_rm()->GetObjectA(Ztl_bstr_t(L"UI/Basic.img/ItemNo")));
    }
    // 4. 计算数字显示（大于60秒显示分钟，小于60秒根据位数调整 X 轴偏移量 nOffset）
    int32_t nOffset = 2;
    if (nSeconds >= 60) {
        nSeconds = nSeconds / 60; // display minutes
    } else if (nSeconds >= 10) {
        nOffset = 15;
    } else {
        nOffset = 22;
    }
    if (nSeconds > 999 || nSeconds <= 0) {
        return;
    }
    // 5. 调用游戏内的绘制函数，把数字直接画到刚才的阴影画布上！
    draw_number_by_image(pNewCanvas, nOffset, 19, nSeconds, g_pPropSecond, 0);

    // insert canvas
    // 6. 将画好数字的新画布重新插入
    pThis->pLayerShadow->InsertCanvas(pNewCanvas, 500, 210, 64);
}


static auto TEMPORARY_STAT__SetLeft = reinterpret_cast<void(__thiscall*)(CTemporaryStatView::TEMPORARY_STAT*, int)>(0x0075DA00);

void __fastcall TEMPORARY_STAT__SetLeft_hook(CTemporaryStatView::TEMPORARY_STAT* pThis, void* _EDX, int tNewLeft) {
    int tDelta = g_tLastUpdate < 0 ? -30 : g_tLastUpdate - CWvsApp::GetInstance()->m_tUpdateTime;
    TEMPORARY_STAT__SetLeft(pThis, pThis->tLeft + tDelta);
}


void AttachTemporaryStatMod() {
    ATTACH_HOOK(CTemporaryStatView__Clear, CTemporaryStatView__Clear_hook);
    ATTACH_HOOK(CTemporaryStatView__Show, CTemporaryStatView__Show_hook);
    ATTACH_HOOK(CTemporaryStatView__Hide, CTemporaryStatView__Hide_hook);
    ATTACH_HOOK(CTemporaryStatView__Update, CTemporaryStatView__Update_hook);
    ATTACH_HOOK(CTemporaryStatView__AdjustPosition, CTemporaryStatView__AdjustPosition_hook);
    ATTACH_HOOK(CWvsContext__SetSkillCooltimeOver, CWvsContext__SetSkillCooltimeOver_hook);
    ATTACH_HOOK(CWvsContext__RemoveSkillCooltimeReset, CWvsContext__RemoveSkillCooltimeReset_hook);
    ATTACH_HOOK(TEMPORARY_STAT__UpdateShadowIndex, TEMPORARY_STAT__UpdateShadowIndex_hook);
    PatchCall(0x0075DCCC, reinterpret_cast<uintptr_t>(&TEMPORARY_STAT__SetLeft_hook)); // CTemporaryStatView::Update
}