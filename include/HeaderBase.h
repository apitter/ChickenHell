#ifndef ___HEADERBASE_HEADER__
#define ___HEADERBASE_HEADER__

#include <assert.h>

#ifndef WIN32
#include <unistd.h>
#else
// windows specifics
#define _WIN32_WINNT _WIN32_WINNT_WIN7
//#define _WIN32_WINNT _WIN32_WINNT_WIN8
#if defined( _MSC_VER )
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS (1)
#endif
// microsoft compiler specifics
#define NOMINMAX
#if _MSC_VER <= 1600            // <= VS 2010 (VC10)
#pragma warning (disable: 4481) // disable warning C4481: nonstandard extension used: override specifier 'override'
#endif
#include <SDKDDKVer.h>
#endif // end microsoft compiler specifics
#include <windows.h> 
#endif

#define FCompare2(a,b) \
      ( ((a)-1.0e-06F)<(b) && ((a)+1.0e-06F)>(b) )

#if YOU_WANT_TO_BUILD_NGL_LIB_UNDER_WINDOWS_PUT_THE_FOLLOWING_CODE_IN_ngl_Types_h_line63
// http://stackoverflow.com/questions/18387640/how-to-deal-with-noexcept-in-visual-studio
#if defined(__clang__)
#if __has_feature(cxx_noexcept)
#define HAS_NOEXCEPT
#endif
#else
#if defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
    defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
#define HAS_NOEXCEPT
#endif
#endif
#ifndef HAS_NOEXCEPT
#ifdef _MSC_VER
#define _XKEYCHECK_H
#endif
#define noexcept
#define constexpr static
#endif
#endif

#include <ngl/Types.h>

// c++ includes
#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>


#ifndef _countof
#include <boost/range.hpp>
#define _countof boost::size
// #define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#include <QtGui/QtEvents>
#include <QColor>
#include <QFont>
#include <QImage>
#include <QPainter>
#include <QApplication>
#include <QTime>


#endif // !___HEADERBASE_HEADER__
