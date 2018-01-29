// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// Disable msvc secure warnings.
#if defined(_WINDOWS) && defined(_MSC_VER)
#if !defined(_CRT_SECURE_NO_WARNINGS) && (_MSC_VER >= 1200)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
//#pragma warning(disable:4996)
#endif
#endif

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ 运行时头文件
#include <iostream>

// TODO:  在此处引用程序需要的其他头文件
#include "./System/Support/YwBase.h"
