#pragma once
#include "debug.h"
#include <cstdint>

/*
* 自动化函数挂钩宏
reinterpret_cast<void>(&TARGET)：自动把你的游戏原版函数指针强转为 void，省去了你每次手动写强转的麻烦。
CastHook(&DETOUR)：将你的 Hook 目标函数转为通用的 void*（下面会详细讲 CastHook 的特殊之处）。
#DETOUR（仅在 _DEBUG 下）：C++ 预处理器的特性，能把你的函数名直接转换成字符串。
如果 Hook 失败，它会弹出一行极其人性化的报错：“Failed to attach detour function 'CWvsApp__ctor_hook' at target address: 0x009CA8A0”。
*/
#ifdef _DEBUG
#define ATTACH_HOOK(TARGET, DETOUR) \
    AttachHook(reinterpret_cast<void**>(&TARGET), CastHook(&DETOUR)) ? true : (ErrorMessage("Failed to attach detour function \"%s\" at target address : 0x%08X.", #DETOUR, TARGET), false)
#else
#define ATTACH_HOOK(TARGET, DETOUR) \
    AttachHook(reinterpret_cast<void**>(&TARGET), CastHook(&DETOUR))
#endif




/*
MEMBER_AT(T, OFFSET, NAME) —— 极其硬核的“内存偏移隐式映射”宏
这是整个头文件里技术含量最高的宏。它是为了解决 C++ 逆向工程中一个极其头疼的问题：
“我知道游戏某个类在某个偏移量（Offset）有个变量，但我没办法重构整个类的结构体，该怎么读写它？”
它利用了微软 VC++ 编译器的独有特性 __declspec(property)，在类的内部虚拟出一个成员变量。

__declspec(property(get = get_##NAME, put = set_##NAME)) T NAME;
声明一个名为 NAME、类型为 T 的虚拟属性。当你代码里写 obj.NAME 时，编译器会自动去调用 get_NAME()；写 obj.NAME = val 时，自动调用 set_NAME()。

get_##NAME() const 与 get_##NAME()
利用 reinterpret_cast<uintptr_t>(this) + OFFSET，直接拿当前对象的首地址（this 指针），硬生生加上你指定的字节偏移量，然后把那块内存强转成类型 T 的引用返回。

set_##NAME(...)
同理，找到偏移后的内存，直接往里写入新值。


实际应用场景：
假设原版游戏里的类 CWvsApp 大小有几个 KB，你想修改偏移量为 0x203C 处的 m_bWindowActive（窗口是否激活，布尔值）。
你不需要在 C++ 里写一堆占位数组，直接在类定义里塞入这个宏：

class CWvsApp {
public:
    MEMBER_AT(bool, 0x203C, m_bWindowActive);
};
在后续代码中，你可以像对待原生成员一样极其自然地操作它：

pThis->m_bWindowActive = true; // 编译器在底层会自动将其转化为：*(bool*)((char*)pThis + 0x203C) = true;


*/
#define MEMBER_AT(T, OFFSET, NAME) \
    __declspec(property(get = get_##NAME, put = set_##NAME)) T NAME; \
    __forceinline const T& get_##NAME() const { \
        return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(this) + OFFSET); \
    } \
    __forceinline T& get_##NAME() { \
        return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + OFFSET); \
    } \
    __forceinline void set_##NAME(const T& value) { \
        *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + OFFSET) = const_cast<T&>(value); \
    } \
    __forceinline void set_##NAME(T& value) { \
        *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + OFFSET) = value; \
    }

// 作用与上面的 MEMBER_AT 相同，但专门用来映射固定长度的数组。
#define MEMBER_ARRAY_AT(T, OFFSET, NAME, N) \
    __declspec(property(get = get_##NAME)) T(&NAME)[N]; \
    __forceinline T(&get_##NAME())[N] { \
        return *reinterpret_cast<T(*)[N]>(reinterpret_cast<uintptr_t>(this) + OFFSET); \
    }


/*
作用：把任意成员函数指针、__fastcall 函数指针，强行转换成通用的 void* 指针。

为什么不直接用 reinterpret_cast<void*>？
在 C++ 标准中，普通指针（void*）和成员函数指针/特定调用约定的函数指针是不兼容的
。如果你直接用 reinterpret_cast 强转一个 __fastcall 的函数，编译器会无情报错。

解决方案（Union Bug）：利用 union（联合体）所有成员共享同一块内存的特性。把函数指针装进 u.fn，然后从 u.p 以 void* 的形式读出来。
这是 C++ 逆向开发中绕过编译器类型检查、强转函数指针的黄金标准方案。


*/
template <typename T>
constexpr auto CastHook(T fn) -> void* {
    union {
        T fn;
        void* p;
    } u;
    u.fn = fn;
    return u.p;
}




// called in injector.cpp -> DllMain
void AttachSystemHooks();

// called in system.cpp -> CreateMutexA_hook
void AttachClientBypass();
void AttachClientHelper();
void AttachClientInlink();
void AttachStringPoolMod();
void AttachSystemOptionMod();
void AttachTemporaryStatMod();
void AttachElementalDamageMod();
void AttachIconIconMod();
void AttachQuickSlot();
void AttachExceptionHandler();

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



bool AttachHook(void** ppTarget, void* pDetour);

void* VMTHook(void* pInstance, void* pDetour, size_t uIndex);

void* GetAddress(const char* sModuleName, const char* sProcName);

void* GetAddressByPattern(const char* sModuleName, const char* sPattern);

void Patch1(uintptr_t pAddress, uint8_t uValue);

void Patch4(uintptr_t pAddress, uint32_t uValue);

void PatchStr(uintptr_t uAddress, const char* sValue);

void PatchJmp(uintptr_t pAddress, uintptr_t pDestination);

void PatchJmp(uintptr_t pAddress, uintptr_t pDestination, size_t uSize);

void PatchNop(uintptr_t pAddress, uintptr_t pDestination);

void PatchCall(uintptr_t pAddress, uintptr_t pDestination);

void PatchRetZero(uintptr_t pAddress);
