#include "bx_str.h"

//#include <locale>


namespace btxon
{

// Replace all `strFrom` of `strLine` to `strTo`
void ReplaceString(bxStrW& strLine, const bxStrW& strFrom, const bxStrW& strTo)
{
	if(strLine.empty() || strFrom == strTo)
		return;

	bxStrW::size_type pos = 0;

	while(bxStrW::npos != (pos = strLine.find(strFrom, pos)))
	{
		strLine.replace(pos, strFrom.length(), strTo);
		pos += (strTo.length() - strFrom.length() + 1);
	}
}

void ReplaceString(bxStrA& strLine, const bxStrA& strFrom, const bxStrA& strTo)
{
	if(strLine.empty() || strFrom == strTo)
		return;

	bxStrA::size_type pos = 0;

	while(bxStrA::npos != (pos = strLine.find(strFrom, pos)))
	{
		strLine.replace(pos, strFrom.length(), strTo);
		pos += (strTo.length() - strFrom.length() + 1);
	}
}

bool IsOnlyWhiteSpace(const bxStr& strString)
{
	bxStr::size_type pos = 0;
	bxStr strFilter(_T(" \t\r\n"));
	pos = strString.find_first_not_of(strFilter);
	return bxStr::npos == pos;
}

void RemoveSpaceChar(bxStrA& strString, unsigned long ulFlag)
{
	bxStrA::size_type pos = 0;
	bxStrA strFilter(" \t\r\n");

	if(ulFlag & RSC_BEGIN)
	{
		pos = strString.find_first_not_of(strFilter);
		if(bxStrA::npos != pos)
			strString.erase(0, pos);
// FIXME
	}
	if(ulFlag & RSC_END)
	{
		pos = strString.find_last_not_of(strFilter);
		if(bxStrA::npos != pos)
			strString.erase(pos + 1);
// FIXME
	}
}

void RemoveSpaceChar(bxStrW& strString, unsigned long ulFlag)
{
	bxStrW::size_type pos = 0;
	bxStrW strFilter(L" \t\r\n");

	if(ulFlag & RSC_BEGIN)
	{
		pos = strString.find_first_not_of(strFilter);
		if(bxStrW::npos != pos)
			strString.erase(0, pos);
		// FIXME
	}
	if(ulFlag & RSC_END)
	{
		pos = strString.find_last_not_of(strFilter);
		if(bxStrW::npos != pos)
			strString.erase(pos + 1);
		// FIXME
	}
}

// void StringToLower(axString& src)
// {
// 	use_facet< ctype<TCHAR> >( locale() ).tolower( &(*src.begin()), &(*src.end()) );
// }

bxStrA ToStrA(const bxStrW& strStr, unsigned int CodePage /*= CP_UTF8*/)
{
#if defined(BX_OS_WIN32)
	int iSize = WideCharToMultiByte(CodePage, 0, (const wchar_t*)strStr.c_str(), -1, NULL, 0, NULL, NULL);
	if(iSize <= 0)
		return "";

	++iSize;
	char* pszStr = new char[iSize];
	if(NULL == pszStr)
		return "";
	ZeroMemory(pszStr, iSize);

	WideCharToMultiByte(CodePage, 0, (const wchar_t*)strStr.c_str(), -1, pszStr, iSize, NULL, NULL);

	bxStrA strRet(pszStr);
	delete[] pszStr;
	pszStr = NULL;

	return strRet;
#elif defined(BX_OS_UNIX_LIKE)
//#   error "should use iconv."
	CodePage = CodePage;
	strStr.c_str();

	// TODO: use iconv.
	return "";

#else
#   error "error."
#endif
}

bxStrW ToStrW(const bxStrA& strStr, unsigned int CodePage /*= CP_UTF8*/)
{
#if defined(BX_OS_WIN32)
	int iSize = MultiByteToWideChar(CodePage, 0, strStr.c_str(), (int)strStr.length(), NULL, 0);
	if(iSize <= 0)
		return L"";

	++iSize;
	wchar_t* pszStr = new wchar_t[iSize];
	if(NULL == pszStr)
		return L"";
	ZeroMemory(pszStr, iSize * sizeof(wchar_t));

	MultiByteToWideChar(CodePage, 0, strStr.c_str(), (int)strStr.length(), pszStr, iSize * sizeof(wchar_t));

	bxStrW strRet(pszStr);
	delete[] pszStr;
	pszStr = NULL;

	return strRet;
#elif defined(BX_OS_UNIX_LIKE)
//#   error "should use iconv."
	CodePage = CodePage;

	strStr.c_str();

	// TODO: use iconv.
	return L"";

#else
#   error "error."
#endif
}



} // namespace btxon
