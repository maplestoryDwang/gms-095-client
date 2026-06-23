#include "pch.h"
#include "debug.h"
#include "hook.h"
#include <detours.h>
#include <psapi.h>

/*
作用：利用微软的 Detours 库，对一个普通的 API 或函数进行标准的 Inline Hook（内联挂钩）。

工作原理：
DetourTransactionBegin()：开启一个 Hook 事务。
DetourUpdateThread()：将当前线程加入事务，确保多线程环境下修改内存时的线程安全。
DetourAttach(ppTarget, pDetour)：核心步骤。它会修改 ppTarget 指向的函数开头，写入一个跳转指令（JMP）到你的自定义函数 pDetour，
同时把原函数的备份地址存回 ppTarget（以便你后续还能调用原函数）。
DetourTransactionCommit()：提交并生效这次修改。
*/
bool AttachHook(void** ppTarget, void* pDetour) {
    LONG result;
    if (result = DetourTransactionBegin(); result != NO_ERROR) {
        DEBUG_MESSAGE("DetourTransactionBegin failed with : %d", result);
        return false;
    }
    if (result = DetourUpdateThread(GetCurrentThread()); result != NO_ERROR) {
        DEBUG_MESSAGE("DetourUpdateThread failed with : %d", result);
        DetourTransactionAbort();
        return false;
    }
    if (result = DetourAttach(ppTarget, pDetour); result != NO_ERROR) {
        DEBUG_MESSAGE("DetourAttach failed with : %d", result);
        DetourTransactionAbort();
        return false;
    }
    if (result = DetourTransactionCommit(); result != NO_ERROR) {
        DEBUG_MESSAGE("DetourTransactionCommit failed with : %d", result);
        DetourTransactionAbort();
        return false;
    }
    return true;
}

/*
作用：对 C++ 虚函数表（Virtual Method Table, VMT）中的某个虚函数进行 Hook。

工作原理：
*static_cast<void***>(pInstance)：获取某个 C++ 类对象的虚函数表指针（vtable）。
vtable[uIndex]：根据索引 uIndex 找到具体的虚函数地址 pTarget。
随后调用前面的 AttachHook，对这个具体的虚函数地址进行拦截，并返回原函数的地址。

*/
void* VMTHook(void* pInstance, void* pDetour, size_t uIndex) {
    void** vtable = *static_cast<void***>(pInstance);
    void* pTarget = vtable[uIndex];
    AttachHook(&pTarget, pDetour);
    return pTarget;
}

/*
作用：通过模块名和导出函数名，动态获取某个 DLL 中的函数地址。

工作原理：先尝试用 GetModuleHandleA 获取已加载模块的句柄，如果没有加载，则用 LoadLibraryA 强制加载它。最后通过 Windows API GetProcAddress 拿到函数的真实内存地址。
*/
void* GetAddress(const char* sModuleName, const char* sProcName) {
    HMODULE hModule = GetModuleHandleA(sModuleName);
    if (!hModule) {
        hModule = LoadLibraryA(sModuleName);
    }
    FARPROC result = GetProcAddress(hModule, sProcName);
    if (!result) {
        DEBUG_MESSAGE("Could not resolve address for %s in module %s", sProcName, sModuleName);
    }
    return reinterpret_cast<void*>(result);
}


static bool HexCharToByte(char c, uint8_t* b) {
    if ('0' <= c && c <= '9')
        *b = c - '0';
    else if ('A' <= c && c <= 'F')
        *b = 10 + (c - 'A');
    else if ('a' <= c && c <= 'f')
        *b = 10 + (c - 'a');
    else
        return false;
    return true;
}

/*
作用：辅助函数。将形如 "55 8B EC ?? ?? 83" 的特征码字符串，解析为字节数组和掩码（Mask）数组。
工作原理：遇到正常的十六进制数（如 55），就转为字节存入 abPattern，并在 abMask 对应位置设为 0xFF（表示必须精确匹配）；
遇到 ?? 通配符，则在 abMask 对应位置设为 0x00（表示任意字节都可以匹配）。
*/
static  size_t ParsePattern(const char* sPattern, uint8_t* abPattern, uint8_t* abMask) {
    size_t i = 0;
    while (*sPattern) {
        if (*sPattern == ' ') {
            sPattern++;
            continue;
        }
        if (sPattern[0] == '?' && sPattern[1] == '?') {
            abMask[i] = 0x00;
        } else {
            uint8_t high, low;
            if (!HexCharToByte(sPattern[0], &high) || !HexCharToByte(sPattern[1], &low)) {
                return 0;
            }
            abPattern[i] = (high << 4) | low;
            abMask[i] = 0xFF;
        }
        sPattern += 2;
        i += 1;
    }
    return i;
}



/*
* 作用：特征码搜索（Pattern Scanning）。在指定的模块内存中，暴力搜索匹配特定机器码指令的地址。

工作原理：
通过 GetModuleInformation 获取目标模块在内存中的起始基址（lpBaseOfDll）和总大小（SizeOfImage）。
调用 ParsePattern 解析特征码。
使用双重循环在整个模块的内存中进行滑动比对。比对时利用 & abMask 过滤掉通配符（??）。
一旦匹配成功，立刻返回该段内存的绝对地址。

*/
void* GetAddressByPattern(const char* sModuleName, const char* sPattern) {
    HMODULE hModule = GetModuleHandleA(sModuleName);
    if (!hModule) {
        hModule = LoadLibraryA(sModuleName);
    }
    MODULEINFO mi;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &mi, sizeof(mi))) {
        DEBUG_MESSAGE("Could not get module information for : %s", sModuleName);
        return nullptr;
    }
    uint8_t* pModuleBase = static_cast<uint8_t*>(mi.lpBaseOfDll);
    size_t uModuleSize = mi.SizeOfImage;

    uint8_t abPattern[1024];
    uint8_t abMask[1024];
    size_t uPatternSize = ParsePattern(sPattern, abPattern, abMask);
    if (uPatternSize == 0) {
        DEBUG_MESSAGE("Could not parse pattern : %s", sPattern);
        return nullptr;
    }

    for (size_t i = 0; i <= uModuleSize - uPatternSize; ++i) {
        size_t j;
        for (j = 0; j < uPatternSize; ++j) {
            if ((pModuleBase[i + j] & abMask[j]) != (abPattern[j] & abMask[j])) {
                break;
            }
        }
        if (j == uPatternSize) {
            return &pModuleBase[i];
        }
    }
    DEBUG_MESSAGE("Could not resolve address for pattern \"%s\" in module %s", sPattern, sModuleName);
    return nullptr;
}

/*
工作原理：因为代码段通常是只读的（PAGE_EXECUTE_READ），直接写入会引发崩溃。
所以先用 VirtualProtect 改为可读可写可执行（PAGE_EXECUTE_READWRITE），写入数据后，再恢复原本的内存保护属性。
*/
void Patch1(uintptr_t pAddress, uint8_t uValue) {
    DWORD flOldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), 1, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *reinterpret_cast<uint8_t*>(pAddress) = uValue;
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), 1, flOldProtect, &flOldProtect);
}

void Patch4(uintptr_t pAddress, uint32_t uValue) {
    DWORD flOldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), 4, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *reinterpret_cast<uint32_t*>(pAddress) = uValue;
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), 4, flOldProtect, &flOldProtect);
}

/*
作用：向指定内存地址写入一段连续的字节集/字符串。常用于替换一整段机器码（Shellcode）。
*/
void PatchStr(uintptr_t pAddress, const char* sValue) {
    size_t uSize = strlen(sValue);
    DWORD flOldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), uSize, PAGE_EXECUTE_READWRITE, &flOldProtect);
    CopyMemory(reinterpret_cast<PVOID>(pAddress), sValue, uSize);
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), uSize, flOldProtect, &flOldProtect);
}


/*
    作用：在指定的地址强行写入一个强制跳转指令（JMP 目标地址）。工作原理：单地址版：先写入 0xE9（32位近跳转的 Opcode），
    随后计算并写入一个 4 字节的相对偏移量（公式：$目标地址 - 当前地址 - 5$）。
    
    带大小版：如果我们要覆盖的原指令比较长（比如原本是一条 7 字节的指令），而 JMP 只占 5 字节，为了防止剩下的 2 字节变成垃圾指令导致程序崩溃，该函数会在写完 JMP 后，
    调用 PatchNop 把后面多余的字节用 NOP 填充掉。

*/
void PatchJmp(uintptr_t pAddress, uintptr_t pDestination) {
    Patch1(pAddress, 0xE9);
    Patch4(pAddress + 1, pDestination - pAddress - 5); // 偏移量
}

void PatchJmp(uintptr_t pAddress, uintptr_t pDestination, size_t uSize) {
    if (uSize < 5) {
        // 处理错误：改用短跳转（JMP rel8）或其它方案
        return;
    }
    PatchJmp(pAddress, pDestination);
    if (uSize > 5) {
        PatchNop(pAddress + 5, pAddress + uSize);
    }
}

/*
作用：将指定范围内的内存全部填充为 0x90（即汇编中的 NOP 指令，什么都不做）。常用于干掉游戏的某个判断逻辑（比如让“减血”或“检测”代码失效）。
*/
void PatchNop(uintptr_t pAddress, uintptr_t pDestination) {
    size_t uSize = pDestination - pAddress;
    DWORD flOldProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), uSize, PAGE_EXECUTE_READWRITE, &flOldProtect);
    FillMemory(reinterpret_cast<PVOID>(pAddress), uSize, 0x90);
    VirtualProtect(reinterpret_cast<LPVOID>(pAddress), uSize, flOldProtect, &flOldProtect);
}

/*
作用：在指定地址强行写入一个函数调用指令（CALL 目标地址）。
工作原理：原理与 PatchJmp 类似，只是将机器码开头改成了 0xE8（32位相对 CALL 的 Opcode）。
*/
void PatchCall(uintptr_t pAddress, uintptr_t pDestination) {
    Patch1(pAddress, 0xE8);
    Patch4(pAddress + 1, pDestination - pAddress - 5);
}

/*
作用：让指定地址的函数立刻返回 0。
工作原理：它通过 PatchStr 写入了三个字节的机器码：\x33\xC0\xC3。
33 C0 对应汇编 XOR EAX, EAX（将返回值寄存器 EAX 清零，即 return 0）。
C3 对应汇编 RET（函数返回）。
常见应用场景：直接废掉游戏的某个检测函数（如 CheckAntiCheat()），让它一进去就直接返回成功（0）。
*/
void PatchRetZero(uintptr_t pAddress) {
    PatchStr(pAddress, "\x33\xC0\xC3");
}
