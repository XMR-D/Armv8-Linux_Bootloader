#include <stdint.h>
#include "serial.h"

//code imported from OS dev page on CRC32 and slightly modified
// calculate a checksum on a buffer -- start address = p, length = bytelength
uint32_t poly8_lookup[256];
extern uint64_t Image_offset;

uint32_t crc32_byte(uint8_t *p, uint64_t bytelength)
{
    uint32_t crc = 0xffffffff;
    while (bytelength != 0)
    {
        crc = poly8_lookup[((uint8_t) crc ^ *(p))] ^ (crc >> 8);
        p++;
        bytelength--;
    }
    return (~crc);
}

//Fill the lookup table -- table = the lookup table base address
void crc32_fill(uint32_t *table)
{
    uint8_t index = 0;
    do
    {
        table[index] = index;
        for(uint8_t z = 8; z; z--)
        {
            table[index] = (table[index] & 1) ? (table[index]>>1) ^ 0xEDB88320 : table[index]>>1;
        }
    }while(++index);
}

uint32_t crc32_check(uint64_t size)
{
    crc32_fill(poly8_lookup);
    uint8_t *p = (uint8_t *) Image_offset;
    return crc32_byte(p, size);
}
