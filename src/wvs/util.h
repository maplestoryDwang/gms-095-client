#pragma once
#include "ztl/ztl.h"
#include <cstdint>
#include <memory>

/*

它的核心作用是：通过硬编码的内存绝对地址，
把游戏原生的 2D 图形渲染引擎（Gr2D）、资源管理器（ResMan）、Wz 档案命名空间（NameSpace）
以及变体数据类型（Variant）包装成友好的 C++ 内联函数（inline）。
这样你在编写其他魔改代码（比如画 UI、读物品属性）时，就能直接调用这些函数，而不需要每次都去写生硬的内存指针转换。

*/



// 1. get_gr() —— 获取 2D 图形渲染引擎
// 解释：地址 0x00C6F430 存放的是游戏屏幕刷新、图层绘制、帧率控制的核心对象指针。
// 我们在前一题看到的 get_gr()->RenderFrame()（渲染一帧画面）和 get_gr()->nextRenderTime 就是通过这个函数拿到的。
inline IWzGr2DPtr& get_gr() {
    return *reinterpret_cast<IWzGr2DPtr*>(0x00C6F430);
}

// 2. get_rm() —— 获取资源管理器
// 作用：获取游戏 资源管理器（IWzResMan）的全局智能指针。
// 解释：地址 0x00C6F434 存放的是负责从外部.wz 档案（如 String.wz、UI.wz）中加载图片、声音、坐标等数据的基础对象。
// 前一题中魔改宠物图标时调用的 get_rm() -> GetObjectA(...) 便是通过它从数据包里捞文件的。
inline IWzResManPtr& get_rm() {
    return *reinterpret_cast<IWzResManPtr*>(0x00C6F434);
}

// 3. get_root() 与 get_sub(int32_t nIdx) —— Wz 目录命名空间
static inline IWzNameSpacePtr& get_root() {
    return *reinterpret_cast<IWzNameSpacePtr*>(0x00C6F43C);
}

static inline IWzNameSpacePtr& get_sub(int32_t nIdx) {
    return reinterpret_cast<IWzNameSpacePtr*>(0x00C6F440)[nIdx];
}

inline IUnknownPtr get_unknown(Ztl_variant_t& v){
    IUnknownPtr result;
    reinterpret_cast<IUnknownPtr*(__cdecl*)(IUnknownPtr*, Ztl_variant_t*)>(0x004176E0)(std::addressof(result), &v); // RVO in client
    return result;
}

inline uint32_t get_int32(Ztl_variant_t& v, uint32_t nDefault) {
    Ztl_variant_t vInt;
    if (V_VT(&v) == VT_EMPTY || V_VT(&v) == VT_ERROR || FAILED(ZComAPI::ZComVariantChangeType(&vInt, &v, 0, VT_I4))) {
        return nDefault;
    } else {
        return V_I4(&vInt);
    }
}

// 用于显示CD 时间
/*
作用：调用游戏位于 0x00965780 的原生绘制函数，用特定的数字切图把一个阿拉伯数字渲染到屏幕上。

参数释义：
pCanvas：画布指针（要在哪里画）。
nLeft, nTop：在屏幕或 UI 上的 (X, Y) 坐标坐标。
nNo：具体要画的数字（例如你的伤害数值 123456，或者血量）。
pBase：数字纹理的基准属性节点。在冒险岛里，数字不是用普通的 TTL 字体打印的，而是从 Wz（比如 UI.wz/NoDamage）里读取 0~9 组成的十张小图片。这个参数就是告诉函数该用哪一套图片样式来画。
nHorzSpace：水平字间距（每个数字之间隔开多少像素）。
*/
inline int32_t draw_number_by_image(IWzCanvasPtr pCanvas, int32_t nLeft, int32_t nTop, int32_t nNo, IWzPropertyPtr pBase, int32_t nHorzSpace) {
    return reinterpret_cast<int32_t (__cdecl*)(IWzCanvasPtr, int32_t, int32_t, int32_t, IWzPropertyPtr, int32_t)>(0x00965780)(pCanvas, nLeft, nTop, nNo, pBase, nHorzSpace);
}