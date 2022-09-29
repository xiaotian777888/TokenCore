#include "eosapi_utils.h"
#include <crypto/base58.h>
#include <crypto/sha256.h>

bool bx_hex2bin(const char* hex_str, unsigned char* buf_data, int buf_size)
{
    int hex_len = (int)strlen(hex_str);

    if (hex_len % 2 != 0)
        return false;
    int bin_size = hex_len / 2;
    if (buf_size < bin_size)
        return false;

    char ch1 = 0;
    char ch2 = 0;
    for (int i = 0; i < bin_size; ++i)
    {
        ch1 = hex_str[i * 2];
        ch2 = hex_str[i * 2 + 1];

        if (ch1 >= 'a' && ch1 <= 'f')
            ch1 = 'A' + (ch1 - 'a');
        if (ch2 >= 'a' && ch2 <= 'f')
            ch2 = 'A' + (ch2 - 'a');

        // 加入更多的错误检查：可能这不是一个合法的HEX字符串。
        if (!(((ch1 >= 'A' && ch1 <= 'F') || (ch1 >= '0' && ch1 <= '9')) && ((ch2 >= 'A' && ch2 <= 'F') || (ch2 >= '0' && ch2 <= '9'))))
        {
            return false;
        }

        buf_data[i] = ((ch1 >= 'A') ? ((ch1 - 'A') + 10) : (ch1 - '0')) * 16 + ((ch2 >= 'A') ? ((ch2 - 'A') + 10) : (ch2 - '0'));
    }

    return true;
}

void bx_memrevert(unsigned char* buf_data, int buf_size) {
    if (NULL == buf_data || 0 == buf_size)
        return;

    unsigned char t;
    int loop = buf_size / 2;
    for (int i = 0; i < loop; ++i) {
        t = buf_data[i];
        buf_data[i] = buf_data[buf_size - 1 - i];
        buf_data[buf_size - 1 - i] = t;
    }
}

#define COS_DEBUG_OUTPUT_BYTE_PER_LINE		16
#define COS_DEBUG_OUTPUT_BYTE_LINE_SPLIT		8
#define LOG_ENTER()								printf("\r\n")			///< 输出换行符
void bx_log_bin(const u8* dat, u16 dataLen)
{
    u16 i = 0;
    u16 iLoop = dataLen / COS_DEBUG_OUTPUT_BYTE_PER_LINE;
    u16 iByteInLastLine = dataLen % COS_DEBUG_OUTPUT_BYTE_PER_LINE;

    u16 x = 0;
    u16 y = 0;

    for (; x < iLoop; ++x)
    {
        printf("%08X  ", x*COS_DEBUG_OUTPUT_BYTE_PER_LINE);
        for (y = 0; y < COS_DEBUG_OUTPUT_BYTE_PER_LINE; ++y)
        {
            if (COS_DEBUG_OUTPUT_BYTE_LINE_SPLIT == y)
            {
                printf("- ");
            }
            printf("%02X ", dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y]);
        }
// #ifndef LOG_SHOW_IN_LCD
        printf("   ");
        for (y = 0; y < COS_DEBUG_OUTPUT_BYTE_PER_LINE; ++y)
        {
            if ((dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y] >= 0x20) && (dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y] <= 0x7e))
            {
                printf("%c", dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y]);
            }
            else
            {
                printf(".");
            }
        }
// #endif
        LOG_ENTER();
    }

    if (iByteInLastLine > 0)
    {
        u16 padLoop = 0;
        u16 paddingLen = (COS_DEBUG_OUTPUT_BYTE_PER_LINE - iByteInLastLine) * 3;
        if (iByteInLastLine <= COS_DEBUG_OUTPUT_BYTE_LINE_SPLIT)
        {
            paddingLen += 2;
        }

        printf("%08X  ", x*COS_DEBUG_OUTPUT_BYTE_PER_LINE);
        for (y = 0; y < iByteInLastLine; ++y)
        {
            if (COS_DEBUG_OUTPUT_BYTE_LINE_SPLIT == y)
            {
                printf("- ");
            }
            printf("%02X ", dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y]);
        }
// #ifndef LOG_SHOW_IN_LCD
        for (padLoop = 0; padLoop < paddingLen; ++padLoop)
        {
            printf(" ");
        }
        printf("   ");

        for (y = 0; y < iByteInLastLine; ++y)
        {
            //	if(isprint((int) (dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y])))
            if ((dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y] >= 0x20) && (dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y] <= 0x7e))
            {
                printf("%c", dat[i + (x * COS_DEBUG_OUTPUT_BYTE_PER_LINE) + y]);
            }
            else
            {
                printf(".");
            }
        }
// #endif
    }
    LOG_ENTER();

    for (i = 0; i < dataLen; ++i) {
        printf("%02x", dat[i]);
    }
    LOG_ENTER();
    LOG_ENTER();
}

bool bx_eos_wif2bin(const string& wif, Binary& bin)
{
    bin.empty();

    if (wif.empty()) {
        return false;
    }

	Binary tmp_bin(37);
	tmp_bin = decode_base58(wif);
    if (tmp_bin[0] != 0x80) {
        // wrong wif.
        return false;
    }

    unsigned char hexChar[33] = { 0 };
    unsigned char digest[32] = { 0 };
    unsigned char res[32] = { 0 };
    unsigned char last4Bytes[4] = { 0 };
    memcpy(hexChar, tmp_bin.data() + 1, 32);
    memcpy(last4Bytes, tmp_bin.data() + 33, 4);
	SHA256_(hexChar, 33, digest);
	SHA256_(digest, 32, digest);
    memcpy(res, digest, 4);

    if (!strncmp(reinterpret_cast<const char*>(res), reinterpret_cast<const char*>(last4Bytes), 4)) {
        // wif hash validate failed.
        return false;
    }

    for (auto c : hexChar) {
        bin.push_back(c);
    }

    return true;
}

