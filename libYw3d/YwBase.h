// Add by Yaukey at 2018-11-01.
// YW Soft Renderer some utility things.

#ifndef __YW_BASE_H__
#define __YW_BASE_H__

// Include base header.
#include <string>
#include <stdint.h>
#include <vector>
#include <list>

#if defined(_WIN32) || defined(WIN32)
    #include <tchar.h>
    #include <wtypes.h>
#endif

#ifndef __amigaos4__
    #include <memory.h>
#else
    #include <exec/types.h>
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

// ------------------------------------------------------------------
// String.
namespace yw
{
    // String define.
    typedef std::string StringA;
    typedef std::wstring StringW;

	#ifndef _UNICODE
        typedef StringA String;
	#else
        typedef StringW String;
	#endif // !_UNICODE
}

// ------------------------------------------------------------------
// Debug things.
#if defined(DEBUG) | defined(_DEBUG)
#endif

// ------------------------------------------------------------------
// Log things.
#if !defined(_WIN32) && !defined(WIN32)
    #define OutputDebugString printf
#else
    #if !defined(OutputDebugString)
        #define OutputDebugString
    #endif
#endif

#ifndef LOG
    #define LOG_A(x, ...)
    #define LOG_W(x, ...)
    #define LOG(x) OutputDebugString(x)
#endif

#ifndef LOGF
    #define LOGF_A(x, ...)
    #define LOGF_W(x, ...)
    #define LOGF(x) OutputDebugString(x)
#endif

#ifndef LOGI
    #define LOGI_A(x, ...)
    #define LOGI_W(x, ...)
    #define LOGI(x) OutputDebugString(x)
#endif

#ifndef LOGIF
    #define LOGIF_A(x, ...)
    #define LOGIF_W(x, ...)
    #define LOGIF(x) OutputDebugString(x)
#endif

#ifndef LOGW
    #define LOGW_A(x, ...)
    #define LOGW_W(x, ...)
    #define LOGW(x) OutputDebugString(x)
#endif

#ifndef LOGWF
    #define LOGWF_A(x, ...)
    #define LOGWF_W(x, ...)
    #define LOGWF(x, ...) OutputDebugString(x)
#endif

#ifndef LOGE
    #define LOGE_A(x, ...)
    #define LOGE_W(x, ...)
    #define LOGE(x, ...) OutputDebugString(x)
#endif

#ifndef LOGEF
    #define LOGEF_A(x, ...)
    #define LOGEF_W(x, ...)
    #define LOGEF(x) OutputDebugString(x)
#endif

#endif // __YW_BASE_H__
