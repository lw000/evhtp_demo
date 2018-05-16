#include "ConverCode.h"
#include <iconv.h>

#include <log4z/log4z.h>
using namespace zsummer::log4z;

static int code_convert(const char *from_charset, const char *to_charset,
	const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	memset(outbuf, 0, outlen);
#ifdef WIN32
	const char *_inbuf = (char*)inbuf;
#else
	char *_inbuf = const_cast<char*>(inbuf);
#endif

	size_t outbytes = outlen;

	size_t ret = -1;
	iconv_t ic = NULL;
	ic = iconv_open(to_charset, from_charset);
	if (ic == (iconv_t *)-1)
	{
		LOGE("iconv_open failed: from:" << from_charset << "to: " << to_charset << ":" << strerror(errno));
		return -1;
	}

	while (inlen > 0)
	{
		ret = iconv(ic, &_inbuf, &inlen, &outbuf, &outbytes);

		if (ret == -1)
		{
			{
				_inbuf += 1;
				inlen -= 1;
				*outbuf = '?';
				outbuf += 1;
				outbytes -= 1;
			}
		}
	}

	ret = iconv_close(ic);
	if (ret == -1)
	{
		LOGE("iconv_close failed: from:" << from_charset << "to: " << to_charset << ":" << strerror(errno));
		return -1;
	}
	return 0;
}

//UTF8->GBK
std::string ConverCode::utf8_to_gbk(const std::string& utf8)
{
	if (utf8.empty()) return "";

	size_t inlen = utf8.size();
	size_t outlen = inlen * 2 + 2;

	char* outbuf = new char[outlen];
	std::string strRet("");
	if (code_convert("utf-8", "gbk", utf8.c_str(), inlen, outbuf, outlen) == 0)
	{
		strRet = outbuf;
	}
	delete[] outbuf;

	return strRet;
}

//gbk->UTF8
std::string ConverCode::gbk_to_utf8(const std::string& gbk)
{
	if (gbk.empty()) return "";

	size_t inlen = gbk.size();

	size_t outlen = inlen * 2 + 2;
	char* outbuf = new char[outlen];

	std::string strRet(gbk);
	if (code_convert("gbk", "utf-8", gbk.c_str(), inlen, outbuf, outlen) == 0)
	{
		strRet = outbuf;
	}
	delete[] outbuf;

	return strRet;
}
