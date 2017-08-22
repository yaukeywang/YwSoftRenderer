/********************************************************************
created:	2010-11-01
filename: 	Base.h
author:		Yaukey

purpose:	Some utility things.
*********************************************************************/

#ifndef _TRF_UTIL_H_
#define _TRF_UTIL_H_

// Disable msvc secure warnings.
#if defined(WIN32) && defined(_MSC_VER)
#if !defined(_CRT_SECURE_NO_WARNINGS) && (_MSC_VER >= 1200)
#define _CRT_SECURE_NO_WARNINGS
//#pragma warning(disable:4996)
#endif
#endif

#include <d3d9.h>
#include <string>
#include "tchar.h"

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
#ifndef TRF_SAFE_DELETE
#define TRF_SAFE_DELETE(p) {if (nullptr != (p)) {delete (p); (p) = nullptr;}}
#endif

#ifndef TRF_SAFE_DELETE_ARRAY
#define TRF_SAFE_DELETE_ARRAY(p) {if (NULL != (p)) {delete [] (p); (p) = nullptr;}}
#endif

#ifndef TRF_SAFE_RELEASE
#define TRF_SAFE_RELEASE(p) {if (nullptr != (p)) {(p)->Release(); (p) = nullptr;}}
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

#endif // _TRF_UTIL_H_
