// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ ����ʱͷ�ļ�
#include <iostream>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "./System/Support/YwBase.h"
