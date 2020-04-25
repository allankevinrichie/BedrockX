﻿#include <lbpch.h>
#include<api\types\types.h>
#include<api\myPacket.h>
#include<stl\varint.h>
#include<api\refl\playerMap.h>
#include<mcapi/Player.h>
#include<mcapi/Certificate.h>
#include<debug\MemSearcher.h>
LBAPI void WPlayer::sendText(string_view text, TextType tp) {
	WBStream txtpkws;
	txtpkws.data.reserve(8 + text.size());
	txtpkws.apply((char)tp, (char)0, MCString(text));
	MyPkt<9> pk{ txtpkws.data };
	v->sendNetworkPacket(pk);
}
static MSearcherEx<NetworkIdentifier> MS_NI;
static MSearcherEx<Certificate*> MS_PC;
THook(void*, "??0ServerPlayer@@QEAA@AEAVLevel@@AEAVPacketSender@@AEAVNetworkHandler@@AEAVActiveTransfersManager@Server@ClientBlobCache@@W4GameType@@AEBVNetworkIdentifier@@EV?$function@$$A6AXAEAVServerPlayer@@@Z@std@@VUUID@mce@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCertificate@@U?$default_delete@VCertificate@@@std@@@std@@H@Z", class Player* a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, int a6, void* a7, __int64 a8, __int64 a9, void* a10_uuid, __int64 a11, void** a12, __int64 a13) {
	void* pCert = *a12;
	auto rv = original(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10_uuid, a11, a12, a13);
	void* pNeti = a7;
	if (MS_NI.myOff == 0) {
		MS_NI.init(a1,[pNeti](void* x) {
				if (McheckRangeR(x, 144) == false)
					return false;
				return SymCall("??8NetworkIdentifier@@QEBA_NAEBV0@@Z", bool, void*, void*)(pNeti, x);
		},2944,192);
		MS_PC.init(
			a1, [pCert](void* x) {
				return Mcompare_pVoid(x, pCert);
			},
			3248, 192);
	}
	return rv;
	#if 0
	uintptr_t thi = (uintptr_t)a1;
	for (uintptr_t off = 8; off <= 4000; off += 8) {
		if (*(void**)(thi + off) == pCert) {
			if (poff_pcert) {
				LOG.p<LOGLVL::Error>("[WPlayer] wtf_cert", poff_pcert, off);
				printf("wtf_cert %lld %lld\n", poff_pcert, off);
			}
			poff_pcert = off;
			continue;
		}
		if (memcmp((void*)(thi + off), pNeti, 144) == 0) {
			if (poff_neti) {
				LOG.p<LOGLVL::Error>("[WPlayer] wtf_neti", poff_neti, off);
			}
			poff_neti = off;
		}
	}
	LOG("[WPlayer] get net",poff_neti,"cert", poff_pcert);
	#endif
	return rv;
}
LBAPI NetworkIdentifier* WPlayer::_getNI() {
	return MS_NI.get(v);
}
LBAPI Certificate* WPlayer::_getCert() {
	return *MS_PC.get(v);
}
LBAPI void WPlayer::kick(const string& reason) {
	LocateS<ServerNetworkHandler>()->disconnectClient(*_getNI(), reason, false);
}
LBAPI void WPlayer::forceKick() {
	LocateS<ServerNetworkHandler>()->onDisconnect(*_getNI());
}
static string getName_real(WPlayer wp) {
	return ExtendedCertificate::getIdentityName(*wp._getCert());
}
static xuid_t getXuid_real(WPlayer wp) {
	auto xuid = ExtendedCertificate::getXuid(*wp._getCert());
	return xuid.size() > 1 ? std::stoull(xuid) : do_hash(getName_real(wp));
}
struct xuidStorage {
	xuid_t val;
	string name;
	xuidStorage(ServerPlayer& sp) {
		val = getXuid_real({ sp });
		name = getName_real({ sp });
	}
	operator xuid_t() {
		return val;
	}
	operator const string&() {
		return name;
	}
};
static playerMap<xuidStorage> xuid_cache;
LBAPI xuid_t WPlayer::getXuid() {
	return xuid_cache[v];
}
LBAPI const string& WPlayer::getName() {
	return xuid_cache[v];
}
LBAPI string WPlayer::getRealName() {
	return ExtendedCertificate::getIdentityName(*_getCert());
}
LBAPI permlvl_t WPlayer::getPermLvl() {
	return v->getCommandPermissionLevel()&0xff;
}
static MSearcherEx<BlockSource*> pPly_BS;
LBAPI class BlockSource& WPlayer::getBlockSource_() {
	if (!pPly_BS.myOff) {
		pPly_BS.init(
			v, [](void* x) {
				return (MreadPtr_Compare((const void***)x, SYM("??_7BlockSource@@6B@")));
			},
			0x348);
	}
	return **pPly_BS.get(v);
	//_ZNK5Actor9getRegionEv
	#if 0
		return *dAccess<BlockSource*, 0x348>(v);
	#endif
}
