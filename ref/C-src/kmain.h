#ifndef KMAIN_H
#define KMAIN_H

struct Info_header
{
    uint16_t HC : 16;
    uint32_t Image_size : 32;
    uint32_t CRC32_checksum : 32;
}__attribute__((packed));

struct magic
{
    char magic[65];
};

#endif
