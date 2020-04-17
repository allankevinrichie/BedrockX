// LightBase.cpp : 定义 DLL 的导出函数。
//

#include<lbpch.h>
#include<iostream>
#include <filesystem>
#include "framework.h"
#include<api/xuidreg/xuidreg.h>
#include<api/event/genericEvent.h>
#include<I18N.h>
LIGHTBASE_API unsigned long long GetBDXAPILevel() {
	return 20200414;
}
Logger<stdio_commit> LOG(stdio_commit{ "[BDX] " });
static void PrintErrorMessage() {
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		std::wcerr << "wtf\n";
		return;
	}
	std::cerr << errorMessageID << std::endl;
	LPWSTR messageBuffer = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM , NULL, errorMessageID,
		MAKELANGID(0x09, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);
	std::wcerr << messageBuffer << std::endl;
	LocalFree(messageBuffer);
}

static void fixupLIBDIR() {
	WCHAR* buffer=new WCHAR[8192];
	auto sz = GetEnvironmentVariableW(TEXT("PATH"), buffer, 8192);
	std::wstring PATH{ buffer, sz };
	sz = GetCurrentDirectoryW(8192, buffer);
	std::wstring CWD{ buffer, sz };
	SetEnvironmentVariableW(TEXT("PATH"), (CWD + L"\\bdxmod;" + PATH).c_str());
	delete[] buffer;
}
static void loadall() {
	static std::vector<std::pair<std::wstring, HMODULE>> libs;
	using namespace std::filesystem;
	create_directory("bdxmod");
	LOG("BedrockX Loaded! version 20200414");
	fixupLIBDIR();
	directory_iterator ent("bdxmod");
	for (auto& i : ent) {
		if (i.is_regular_file() && i.path().extension() == ".dll") {
			auto lib = LoadLibrary(i.path().c_str());
			if (lib) {
				LOG("loaded", canonical(i.path()));
				libs.push_back({ std::wstring{ i.path().c_str() }, lib });
			}
			else {
				LOG.p<LOGLVL::Error>("Error when loading", i.path());
				PrintErrorMessage();
			}
		}
	}
	for (auto& [name, h] : libs) {
		auto FN = GetProcAddress(h, "onPostInit");
		if (!FN) {
			//std::wcerr << "Warning!!! mod" << name << " doesnt have a onPostInit\n";
		}
		else {
			try {
				((void (*)()) FN)();
			}
			catch (...) {
				std::wcerr << "Error!!! mod" << name << " throws an exception when onPostInit\n";
				exit(1);
			}
		}
	}
	libs.clear();
}
namespace GUI {
	void INIT();
};
void FixUpCWD() {
	string buf;
	buf.assign(8192, '\0');
	GetModuleFileNameA(nullptr, buf.data(), 8192);
	buf = buf.substr(0, buf.find_last_of('\\'));
	SetCurrentDirectoryA(buf.c_str());
}
#include<api\scheduler\scheduler.h>
void startWBThread();
static void entry(bool fixcwd) {
	if (fixcwd)
		FixUpCWD();
	#ifdef TRACING_ENABLED
	DOG_INIT();
	#endif
	XIDREG::initAll();
	GUI::INIT();
	I18N::InitAll();
	loadall();
	PostInitEvent::_call();
	PostInitEvent::_removeall();
	addListener([](ServerStartedEvent&) { 
		startWBThread();
		WItem::procoff();
		LOG("Thanks www.rhymc.com for supporting this project");
		LOG(u8"感谢旋律云MC(rhymc)对本项目的支持");
	},EvPrio::LOW);
}
//#include<stl\format.h>
//#include<stl\WorkerPool.h>
THook(int, "main", int a, void* b) {
	std::ios::sync_with_stdio(false);
	system("chcp 65001");
	#if 0
	WorkerPool<void(*)(int),int> wp;
	for (int i = 0; i < 2; ++i) {
		wp.registerWorker([](int z) {
			Sleep(z * 1000);
			LOG(z);
		});
	}
	for (int i = 0; i < 10; ++i) {
		wp.pushWork(std::move(i));
	}
	while (1)
		Sleep(10000);
	#endif
	entry(a>1);
	return original(a, b);
}
