﻿#include <lbpch.h>
#include<api\types\types.h>
#include<mcapi/Actor.h>
#include<debug/MemSearcher.h>
#include<mcapi/Level.h>
static MSearcherEx<unsigned long long> wa_getrtid;
unsigned long long WActor::getRuntimeID() {
	if (!wa_getrtid.myOff) {
		auto plvl=LocateS<ServerLevel>::_srv;
		wa_getrtid.init(
			v, [plvl,base=this->v](void* x) -> bool {
				uintptr_t xx = (uintptr_t)x;
				if ((xx & 7))
					return false;
				return plvl->getRuntimeEntity({ *(u64*)x }, false) == base;
			}, 159 * 8);
	}
	return *wa_getrtid.get(v);
}
static uintptr_t poff_dim;
LBAPI WDim WActor::getDim() {
	if (!poff_dim) {
		char filler[256];
		SymCall("??0ActorCommandOrigin@@QEAA@AEAVActor@@@Z", void, char*, Actor*)(filler, v);
		void* get = SymCall("?getDimension@ActorCommandOrigin@@UEBAPEAVDimension@@XZ", void*, char*)(filler);
		void** myMEM = (void**)v;
		for (int i = 96; i < 116; ++i) {
			if (myMEM[i] == get) {
				if (poff_dim) {
					printf("wtf actor %lld %d\n", poff_dim, i * 8);
				}
				poff_dim = (size_t)i * 8;
			}
		}
	}
	uintptr_t ptr = (uintptr_t)v;
	ptr += poff_dim;
	return WDim(**(Dimension**)ptr);
}
LBAPI int WActor::getDimID() {
	return v->getDimensionId();
}
LBAPI void WActor::teleport(Vec3 to, int dimid) {
	if (getDimID()!=dimid)
		SymCall("?teleport@TeleportCommand@@AEBAXAEAVActor@@VVec3@@PEAV3@V?$AutomaticID@VDimension@@H@@@Z", void, void*, Actor*, Vec3, Vec3*, int)(NULL, v, to, &to, dimid);
	SymCall("?teleport@TeleportCommand@@AEBAXAEAVActor@@VVec3@@PEAV3@V?$AutomaticID@VDimension@@H@@@Z", void, void*, Actor*, Vec3, Vec3*, int)(NULL, v, to, &to, dimid);
}
