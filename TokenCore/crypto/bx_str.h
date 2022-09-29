#ifndef __BX_STRING_H__
#define __BX_STRING_H__

#include "bx_plat.h"
#include "bx_str.h"

#ifdef BX_OS_WIN32
#	pragma warning(push)
#	pragma warning(disable:4995) /* warning C4995: 'swprintf': name was marked as #pragma deprecated */
#	include <string>
#	pragma warning(pop)

#else
#   define CP_ACP       0
#   define CP_UTF8      65001
#endif


#include <vector>
#include <iostream>
#include <sstream>

namespace btxon
{

#ifndef bxStr
#	if defined(UNICODE) || defined(_UNICODE)
		typedef std::basic_string<wchar_t>	bxStr;
#		define bxOStrStream			std::wostringstream
#		define bxCout	std::wcout
#	else
		typedef std::basic_string<char>		bxStr;
#		define bxOStrStream			std::ostringstream
#		define bxCout std::cout
#	endif

	typedef std::basic_string<char>		bxStrA;
	typedef std::basic_string<wchar_t>	bxStrW;
#	define ENDL		std::endl

#	define bxOStrStreamA			std::ostringstream
#	define bxOStrStreamW			std::wostringstream
	typedef std::vector<bxStr>	bxStrs;

#endif


#define RSC_BEGIN	0x01
#define RSC_END		0x02
#define RSC_ALL		RSC_BEGIN | RSC_END

// Remove white-space character at special location. (Space/TAB, etc.)
// Here special location maybe begin or/and end of a string.
void RemoveSpaceChar(bxStrA& strString, unsigned long ulFlag = RSC_ALL);
void RemoveSpaceChar(bxStrW& strString, unsigned long ulFlag = RSC_ALL);

// Does the string contained only white space (" \t\r\n")
bool IsOnlyWhiteSpace(const bxStr& strString);

// Replace all `strFrom` of `strLine` to `strTo`
void ReplaceString(bxStrA& strLine, const bxStrA& strFrom, const bxStrA& strTo);
void ReplaceString(bxStrW& strLine, const bxStrW& strFrom, const bxStrW& strTo);

//void StringToLower(stdString& src );

// Convert between MultiByte and WideChar.
static inline bxStrA ToStrA(const bxStrA& strStr)
{
	return strStr;
}
static inline bxStrA ToStrA(const bxStrA& strStr, unsigned int /*CodePage*/)
{
	return strStr;
}
bxStrA ToStrA(const bxStrW& strStr, unsigned int CodePage = CP_UTF8);

static inline bxStrW ToStrW(const bxStrW& strStr)
{
	return strStr;
}
static inline bxStrW ToStrW(const bxStrW& strStr, unsigned int /*CodePage*/)
{
	return strStr;
}
bxStrW ToStrW(const bxStrA& strStr, unsigned int CodePage = CP_UTF8);

#if defined(UNICODE) || defined(_UNICODE)
static inline bxStr ToStr(const bxStrA& strStr, UINT CodePage = CP_UTF8)
{
	return ToStrW(strStr, CodePage);
}
static inline bxStr ToStr(const bxStrW& strStr, UINT CodePage = CP_UTF8)
{
	return ToStrW(strStr, CodePage);
}
#else
static inline bxStr ToStr(const bxStrA& strStr, unsigned int CodePage = CP_UTF8)
{
	return ToStrA(strStr, CodePage);
}
static inline bxStr ToStr(const bxStrW& strStr, unsigned int CodePage = CP_UTF8)
{
	return ToStrA(strStr, CodePage);
}
#endif

} // namespace btxon

#endif // __BX_STRING_H__
