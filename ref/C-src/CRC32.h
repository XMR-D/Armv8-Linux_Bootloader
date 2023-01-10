#ifndef CRC32_H
#define CRC32_H

uint32_t crc32_byte(uint8_t *p, uint64_t bytelength);
void crc32_fill(uint32_t *table);
uint32_t crc32_check(uint64_t size);

#endif  /* CRC32_H ! */
