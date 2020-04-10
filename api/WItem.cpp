#include <lbpch.h>
#include<api\types\types.h>
#include<api\event\genericEvent.h>
#include<debug/MemSearcher.h>
static bool inMagic;
static MSearcher<unsigned char, 1, 34, 96> MS_WCNT;
unsigned char WItem::getCount() const {
	return MS_WCNT.get(v);
}
THook(void*, "?_setItem@ItemStackBase@@IEAA_NH@Z", class ItemStack* a, int b) {
	if (inMagic) {
		unsigned char pay = 114;
		MS_WCNT.Init(a, &pay);
		LOG("[WItem] offset", MS_WCNT._Off);
		return nullptr;
	}
	return original(a, b);
}
#include<random>
static void procoff() {
	inMagic = true;
	char filler[700];
	memset(filler, 0xff, sizeof(filler));
	SymCall("?init@ItemStackBase@@IEAAXHHH@Z", void, void*, int, int, int)(filler, 0, 114, 0);
	inMagic = false;
}
void WItem::determine_off() {
	addListener([](ServerStartedEvent&) {
			procoff();
	});
}
