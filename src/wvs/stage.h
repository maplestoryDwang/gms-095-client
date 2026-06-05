#pragma once
#include "ztl/ztl.h"
#include "wvs/gobj.h"
#include "wvs/msghandler.h"


class CStage : public IGObj, public IUIMsgHandler, public INetMsgHandler, public ZRefCounted {
public:
    CStage() = default;
    virtual ~CStage() override {
        reinterpret_cast<void(__thiscall*)(CStage*)>(this);
    }
    virtual void Init(void* pParam) {
        ;
    }
    virtual void Close() {
        ;
    }
};

static_assert(sizeof(CStage) == 0x18);


/// 00438CB0
/*
ZRef<CStage> *__cdecl get_stage()
{
  return &g_pStage;
}
*/


// 相当于重写了get_stage方法，直接返回了CStage对象 这个对象放在0x00C6B638 是一个全局变量
inline ZRef<CStage>& get_stage() {
    return *reinterpret_cast<ZRef<CStage>*>(0x00C6B638);
}

// 方法set_stage(CStage *pStage, void *pParam
inline void set_stage(CStage* pStage, void* pParam) {
    reinterpret_cast<void(__cdecl*)(CStage*, void*)>(0x00719C30)(pStage, pParam);
}