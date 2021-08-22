
#include <stdlib.h>
#include "StringHash.h"


static uint32_t gLower[256] =
{
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,  91,  92,  93,  94,  95,
	96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

static uint32_t gHashBuffer[0x500];

static bool gHashBufferInited = false;
void initStringHashBuffer()
{
	if (gHashBufferInited)return;
	uint32_t seed = 0x00100001;

	memset(gHashBuffer, 0, sizeof(gHashBuffer));

	for (uint32_t index1 = 0; index1 < 0x100; index1++)
	{
		for (uint32_t index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
		{
			uint32_t temp1, temp2;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp1 = (seed & 0xFFFF) << 0x10;

			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp2 = (seed & 0xFFFF);

			gHashBuffer[index2] = (temp1 | temp2);
		}
	}

	gHashBufferInited = true;
}


// 性能：10个大写字符，调用一次耗时：0.000716毫秒
// 环境：Release版,CPU Athon 1800+ ,Mem 1G
unsigned int hashString(const char* str)
{
	const uint32_t type = 0;
	uint32_t seed1 = 0x7FED7FED;
	uint32_t seed2 = 0xEEEEEEEE;
	uint32_t ch;
	const uint8_t* ustr = (const uint8_t*)str;

	if (!gHashBufferInited)
		initStringHashBuffer();


	while (*ustr != 0)
	{
		ch = gLower[*ustr++];
		seed1 = gHashBuffer[(type << 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
	}

	return seed1;
}

/// 哈希一个二进制
uint32_t hashBinary(const char* bin, uint32_t len)
{
	uint32_t hash = 5381;
	for (uint32_t n = 0; n < len; n++)
	{
		hash += (hash << 5) + bin[n];
	}

	return (hash & 0x7FFFFFFF);
}

bool makeMap(unsigned char* buffer, unsigned int size, unsigned int key)
{
	uint64_t seed2 = 0xEEEEEEEEEEEEEEEEULL;
	uint64_t seed1 = (key ^ 0xEEEEEEEEULL);
	seed1 <<= 32;
	seed1 |= ~(key ^ 0xEEEEEEEEULL);
	uint64_t* pos = (uint64_t*)buffer;
	uint32_t size8 = size >> 3;
	uint32_t size_trail = size & 7;
	uint64_t ch;

	initStringHashBuffer();

	while (size8-- > 0)
	{
		seed2 += ((uint64_t*)gHashBuffer)[0x200 + (seed1 & 0x7F)];
		ch = *pos ^ (seed1 + seed2);
		seed1 = ((~seed1 << 0x2A) + 0x1111111111111111ULL) | (seed1 >> 0x16);
		seed2 = /*ch + */seed2 + (seed2 << 0xA) + 3;
		*pos++ = ch;
	}

	uint8_t* pos_trail = (uint8_t*)pos;
	while (size_trail-- > 0)
		*pos_trail++ ^= ((uint8_t*)&seed1)[size_trail];

	return true;
}


uint32_t crc32_str(const char* buf)
{
	uint32_t crc = 0xFFFFFFFF;
	while (*buf)
	{
		crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
	}
	return crc ^ 0xffffffffL;
}


static unsigned int g_crypt_table[0x500];

unsigned int* buildCryptTable()
{
	static int s_init_flag = 0;

	unsigned int seed = 0x00100001;
	unsigned int index1 = 0;
	unsigned int index2 = 0;
	int i;
	unsigned int temp1;
	unsigned int temp2;

	if (s_init_flag == 0)
	{
		memset(g_crypt_table, 0, sizeof(g_crypt_table));

		for (index1 = 0; index1 < 0x100; index1++)
		{
			for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
			{
				seed = (seed * 125 + 3) % 0x2AAAAB;
				temp1 = (seed & 0xFFFF) << 0x10;

				seed = (seed * 125 + 3) % 0x2AAAAB;
				temp2 = (seed & 0xFFFF);

				g_crypt_table[index2] = (temp1 | temp2);
			}
		}
		s_init_flag = 1;
	}
	return g_crypt_table;
}

void encodeDataWow(void* pData, unsigned int dataLen, unsigned int seed)
{
	/* some common variables. */
	unsigned int seed2 = 0xEEEEEEEE;
	unsigned int ch;
	unsigned int* in_buf = (unsigned int*)pData;
	const unsigned int* crypto_table = g_crypt_table;
	unsigned int in_size = 0;

	for (in_size = dataLen; in_size >= 4; in_size -= 4)
	{
		seed2 += crypto_table[0x400 + (seed & 0xFF)];
		ch = *in_buf ^ (seed + seed2);
		seed = ((~seed << 0x15) + 0x11111111) | (seed >> 0x0B);
		seed2 = *in_buf + seed2 + (seed2 << 5) + 3;
		*in_buf++ = ch;
	}
}

void decodeDataWow(void* pData, unsigned int dataLen, unsigned int seed)
{
	unsigned int seed2 = 0xEEEEEEEE;
	unsigned int ch;
	unsigned int* in_buf = (unsigned int*)pData;
	const unsigned int* crypto_table = g_crypt_table;
	unsigned int in_size = 0;

	/* we're processing the data 4 bytes at a time. */
	for (in_size = dataLen; in_size >= 4; in_size -= 4)
	{
		seed2 += crypto_table[0x400 + (seed & 0xFF)];
		ch = *in_buf ^ (seed + seed2);
		seed = ((~seed << 0x15) + 0x11111111) | (seed >> 0x0B);
		seed2 = ch + seed2 + (seed2 << 5) + 3;
		*in_buf++ = ch;
	}
}