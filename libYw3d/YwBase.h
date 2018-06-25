// Add by Yaukey at 2010-11-01.
// YW Soft Renderer some utility things.

#ifndef __YW_BASE_H__
#define __YW_BASE_H__

// Include base header.
#include <string>
#include <stdint.h>
#include <vector>
#include <list>
#include <wtypes.h>

#ifdef WIN32
#include <tchar.h>
#endif

// ------------------------------------------------------------------
// Assert utilities.
#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

// ------------------------------------------------------------------
// Delete or release.
#ifndef YW_SAFE_DELETE
#define YW_SAFE_DELETE(p) {if (nullptr != (p)) {delete (p); (p) = nullptr;}}
#endif

#ifndef YW_SAFE_DELETE_ARRAY
#define YW_SAFE_DELETE_ARRAY(p) {if (nullptr != (p)) {delete [] (p); (p) = nullptr;}}
#endif

#ifndef YW_SAFE_RELEASE
#define YW_SAFE_RELEASE(p) {if (nullptr != (p)) {(p)->Release(); (p) = nullptr;}}
#endif

#ifndef YW_SAFE_RELEASE_DELETE
#define YW_SAFE_RELEASE_DELETE(p) {if (nullptr != (p)) {(p)->Release(); delete (p); (p) = nullptr;}}
#endif

// ------------------------------------------------------------------
// Unicode.
#ifdef _UNICODE
#ifndef _T
#define _T(x) L##x
#endif // !_T
#else
#ifndef _T
#define _T(x) x
#endif // !_T
#endif // !_UNICODE

// String.
namespace yw
{
	#ifndef _UNICODE
	#define std_tstring(x) std::##x
	#else
	#define std_tstring(x) std::w##x
	#endif // !_UNICODE

	typedef std_tstring(string) String;
}

//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
#endif

#endif // __YW_BASE_H__
