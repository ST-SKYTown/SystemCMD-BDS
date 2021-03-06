#pragma once
#define WIN32_LEAN_AND_MEAN
#define PCH_INCLUDED
// 当前版本
// Windows 头文件
#include <windows.h>
// C++ 标准库 & 头文件
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <time.h>

#pragma warning(disable:4996)

using VA = unsigned __int64;
using RVA = unsigned int;
template<typename Type>
using Ptr = Type*;

extern "C" {
	_declspec(dllimport) int HookFunction(void* oldfunc, void** poutold, void* newfunc);
	_declspec(dllimport) void* GetServerSymbol(char const* name);
}
template<typename ret, typename... Args>
inline ret SYMCALL(const char* fn, Args... args) {
	return ((ret(*)(Args...))GetServerSymbol(fn))(args...);
}
struct HookRegister {
	HookRegister(void* address, void* hook, void** org) {
		if (HookFunction(address, org, hook) != 0) printf("FailedToHook: %p\n", address);
	}
	HookRegister(char const* sym, void* hook, void** org) {
		auto found = GetServerSymbol(sym);
		if (found == nullptr) printf("FailedToHook: %p\n", sym);
		if (HookFunction(found, org, hook)!= 0) printf("FailedToHook: %s\n", sym);
	}
};

std::string getTime();

/* *********************************  定义宏区  *********************************** */
#define SYMHOOK(name,ret,sym,...)				\
struct name {									\
	typedef ret(*original_type)(__VA_ARGS__);	\
	static original_type& _original() {			\
		static original_type storage;			\
		return storage;							\
	}											\
	template <typename... T>					\
	static ret original(T&&... params) {		\
		return _original()(std::forward<T>(params)...);\
	}											\
	static ret _hook(__VA_ARGS__);				\
};												\
HookRegister name{sym,&name::_hook,(void**)&name::_original()};\
ret name::_hook(__VA_ARGS__)

#define _U(x) u8 ## x

// PR(0, u8"标准流输出"<<u8"!!!")
#define PR(type, ...)\
	if (type == 0)\
		std::cout << "[" << getTime() << " INFO][SystemCMD] " << __VA_ARGS__ << endl;\
	else if (type == 1)\
		std::cout << "[" << getTime() << " WARN][SystemCMD] " << __VA_ARGS__ << endl;\
	else if (type == 2)\
		std::cout << "[" << getTime() << " ERROR][SystemCMD] " << __VA_ARGS__ << endl;\
	else if (type == 3)\
		std::cout << "[" << getTime() << " DEBUG][SystemCMD] " << __VA_ARGS__ << endl;\
	else\
		std::cout << __VA_ARGS__ << endl

// 向游戏内玩家发送消息
#define SENDTEXT(name,playername,...)\
	ostringstream name;\
	name << __VA_ARGS__;\
	sendText(playername,name.str());

// 运行游戏后台命令
#define RUNCMD(name,...)\
	ostringstream name;\
	name << __VA_ARGS__;\
	runcmd(name.str());

// 运行系统命令
#define RUNSYSTEMCMD(name,...)\
	ostringstream name;\
	name << __VA_ARGS__;\
	system(name.str().data());
