/********************************************************************
created:	2010-11-01
filename: 	Base.h
author:		Yaukey

purpose:	Some utility things.
*********************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

//// Disable msvc secure warnings.
//#if defined(WIN32) && defined(_MSC_VER)
//#if !defined(_CRT_SECURE_NO_WARNINGS) && (_MSC_VER >= 1200)
//#define _CRT_SECURE_NO_WARNINGS
////#pragma warning(disable:4996)
//#endif
//#endif

#include <string>
#include <tchar.h>
#include <wtypes.h>
#include <stdint.h>

#include <vector>
#include <list>

//////////////////////////////////////////////////////////////////////////
#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) || defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef YW_SAFE_DELETE
#define YW_SAFE_DELETE(p) {if (nullptr != (p)) {delete (p); (p) = nullptr;}}
#endif

#ifndef YW_SAFE_DELETE_ARRAY
#define YW_SAFE_DELETE_ARRAY(p) {if (NULL != (p)) {delete [] (p); (p) = nullptr;}}
#endif

#ifndef YW_SAFE_RELEASE
#define YW_SAFE_RELEASE(p) {if (nullptr != (p)) {(p)->Release(); (p) = nullptr;}}
#endif

#ifndef YW_SAFE_RELEASE_DELETE
#define YW_SAFE_RELEASE_DELETE(p) {if (nullptr != (p)) {(p)->Release(); delete (p); (p) = nullptr;}}
#endif

//////////////////////////////////////////////////////////////////////////
// String.
namespace yw
{
	#ifndef _UNICODE
	#define std_tstring(x) std::##x
	#else
	#define std_tstring(x) std::w##x
	#endif

	typedef std_tstring(string) String;
}

//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x) x;
//#define HR(x)                                          \
//{                                                      \
//	HRESULT hr = x;                                    \
//	if(FAILED(hr))                                     \
//	{                                                  \
//		DXTrace(__FILE__, __LINE__, hr, _T(#x), TRUE); \
//	}                                                  \
//}
#endif
#else
#ifndef HR
#define HR(x) x;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// We do not want user to change the window size and maximize the window,
// we use WS_YW_RENDERER_FIXED_WINDOW instead of WS_OVERLAPPEDWINDOW.
#define WS_YW_RENDERER_FIXED_WINDOW (WS_OVERLAPPED     | \
                                     WS_CAPTION        | \
                                     WS_SYSMENU        | \
                                     WS_MINIMIZEBOX)
                                     //WS_THICKFRAME   (no sizing border)
                                     //WS_MAXIMIZEBOX  (no maximize button)

#endif // __UTIL_H__
