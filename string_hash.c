#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(bswap32)

#if defined(__builtin_bswap32)

#define bswap32(x) __builtin_bswap32(x)
#define bswap64(x) __builtin_bswap642(x)

#elif defined (_MSC_VER)

#define bswap32(x) _byteswap_ulong(x)
#define bswap64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>
#define bswap32(x) OSSwapInt32(x)
#define bswap64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap32(x) BSWAP_32(x)
#define bswap64(x) BSWAP_64(x)

#elif defined(__FreeBSD__) || defined(__DragonFly__)

#include <sys/endian.h>

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap32(x) swap32(x)
#define bswap64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>

#elif defined(__ICC) || defined(__INTEL_COMPILER)

#define bswap32(x) _bswap(x)
#define bswap64(x) _bswap64(x)

#else

uint32_t bswap32(uint32_t x)
{
    // when ROR/ROL are available, modern compilers optimize below code
    x = ((x >> 8) & 0x00ff00ff) | ((x <<8 ) & 0xff00ff00);
    return (x >> 16) | (x << 16);
}

uint64_t bswap64(uint64_t x)
{
    // when ROR/ROL are available, modern compilers optimize below code
    x = ((x >> 8) & 0x00ff00ff00ff00ff) | ((x << 8) & 0xff00ff00ff00ff00);
    x = ((x >> 16) & 0x0000ffff0000ffff) | ((x << 16) & 0xffff0000ffff0000);
    return (x >> 32) | (x << 32);
}

#endif

#endif // defined(bswap32)


/*
  The sequence in the mixing is a multiplication that produces density in the upper part
  of the hash value, followed by a small shift xor to propagate density towards 
  the bottom of each byte that makes up the hash and finally followed by bswap
  to move the density from upper part to lower part.

  Multiplication is a powerful and fast mixing instruction that is widely supported in hardware.
  The main property of multiplication constant is that it has to be coprime with 2
  to keep it bijective. That means any odd value is a possible candidate, however
  the value chosen was selected to produce the least deviation from a perfect avalanche
  effect on various 4 byte strings of interest.

  The shift xor moves the high density from the top of each byte towards its bottom.
  Larger values than 5 would move the density too far for practical reasons, while
  lower values than 5 do not move it far enough for a good distribution. Therefore 5 is used.

  Finally the byte swap prepares the hash value for either modulo operations for hash tables,
  or for the next hash value computation iteration.
*/

#define HASH32_MIX(_h) \
    (_h) *= 0xe22132c5UL; \
    (_h) ^= (_h) >> 5; \
    (_h) = bswap32((_h));

uint32_t hash32(const uint8_t *data, size_t size, uint32_t hash)
{
    size_t u = size >> 2;

    while (u--)
    {
        hash ^= *(const uint32_t *)data;
        data += 4;
        HASH32_MIX(hash);
    }

    if ((u = (size & 3)))
    {
        hash ^= *(const uint32_t *)data & ((1 << (u << 3)) - 1);
        HASH32_MIX(hash);
    }

    return hash;
}


int main (int argc, const char *argv[])
{
    uint32_t u, h32;

    for (u=0; u<(uint32_t)argc; ++u)
    {
        h32 = hash32((const uint8_t *)argv[u], strlen(argv[u]), 0);
        printf("%08X %s\n", h32, argv[u]);
    }
    return 0;
}
