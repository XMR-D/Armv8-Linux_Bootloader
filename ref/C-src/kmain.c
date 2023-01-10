#include "serial.h"
#include "tools.h"
#include "commands.h"
#include "CRC32.h"
#include "rsa.h"
#include "Open-src/aes.h"
#include "Open-src/sha256.h"
#include "Open-src/bn.h"
#include "kmain.h"

//booting arm procedure

//buffer used to store the command.
uint32_t size = 512;
char cmd[512] = {0};
extern char *args[5];


uint64_t Image_offset = 0x45800000;
struct Info_header *infos = (void *) 0x4580fe44;
uint64_t (*boot)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = (void *) 0x45800000; //incrediboot boot(x0, x1, x2, x3, x4, x5)
struct magic *magic = (void *) 0x40170000;

void My_CLI(void)
{
    uint8_t c = 0;
    uint32_t i = 0;
    uint64_t count = 1;
    puts("\033[0;33mMy_pikaboot > \033[0m");
    wait_unbusy();
    while (c != '\r')
    {
        c = getc();
        if (c == 127)
        {
            if (count == 1)
                continue;
            else
            {
                puts("\b \b");
                i--;
                cmd[i] = 0;
                count--;
            }
        }
        else if (c == ' ')
        {
            count++;
            putc(c);
            cmd[i] = 0;
            i++;
        }
        else
        {
            count++;
            putc(c);
            cmd[i] = c;
            i++;
        }
    }
    putc('\n');
}

uint8_t get_argc()
{
    uint8_t count = 0;
    for(uint8_t i = 0; i < 5; i++)
    {
        if (is_printable(args[i][0]))
        {
            count++;
        }
    }
    return count;
}

void aes_interface(char *key, uint64_t offset1, uint64_t offset2, uint8_t mode)
{
    char rkey[129] = {0};
    offset2 -= 12;
    aes_key_schedule_128((uint8_t *) key, (uint8_t *) rkey);
    int64_t len = offset2 - offset1;
    while(len > 0)
    {
        uint8_t *to_decypher = (uint8_t *) offset1;
        if(mode)
            aes_decrypt_128((uint8_t *) rkey, to_decypher, to_decypher);
        else
            aes_encrypt_128((uint8_t *) rkey, to_decypher, to_decypher);
        offset1 += 16;
        len -= 16;
    }
}

//call boot() with dtbval to save the device tree
void kmain(uint32_t dtbval)
{
    clear(512);
    uint8_t argc = 0;
    uint64_t savedcrc32 = infos->CRC32_checksum;
    infos->CRC32_checksum = 0;
    uint64_t val = crc32_check(infos->Image_size);
    putc('\n');
    if (savedcrc32 != val)
    {
        puts("\033[0;31m");
        put_epika();
        puts("\033[0;33m [WARN]\033[0m : \033[0;31mIMAGE FILE INTEGRITY FAULT STARTING EMERGENCY BOOT.\n\033[0m");
        Send_file((void *) Image_offset);
        boot(dtbval, 0, 0, 0, 0, Image_offset);
    }
    put_pika();
    puts("\033[0;32m [SUCC]\033[0m : IMAGE FILE CHECKSUM CHECK COMPLETE\n");
    puts("\033[0;33m [TEST]\033[0m : SIGNATURE TESTING\n");
    int check = RSA_Sig_Check();
    if (check == 1)
        puts("\033[0;32m [SUCC]\033[0m : SIGNATURE VERIFIED, ALLOWED TO RUN.");
    else
    {
        puts("\033[0;31m [ERR]\033[0m : WRONG SIGNATURE EXITING NOW.");
        return;
    }
    puts("\n");
    while (1)
    {
        clear(size);
        My_CLI();
        if (strcmp(cmd, "md"))
        {
            parsing_cmd();
            argc = get_argc();
            mem_dump(argc);
        }
        else if (strcmp(cmd, "memtest"))
        {
            parsing_cmd();
            argc = get_argc();
            mem_test(args[1], argc);
        }
        else if (strcmp(cmd, "boot\r"))
        {
            boot(dtbval, 0, 0, 0, 0, Image_offset);
        }
        else if (strcmp(cmd, "help\r"))
            help();
        else if (strcmp(cmd, "eboot\r"))
        {
            Send_file((void *) Image_offset);
            boot(dtbval, 0, 0, 0, 0, Image_offset);
        }
        else
        {
            puts("Error: command not found \n");
            puts("(Type: \"help\" to display availables commands)\n");
        }
    }
}

char *rev(char (*ret)[], unsigned char *src, uint64_t size)
{
    int64_t j = 0;
    for (int64_t i = size-1; i >= 0;)
    {
        (*ret)[j] = src[i-1];
        (*ret)[j+1] = src[i];
        j += 2;
        i -= 2;
    }
    return (char *) ret;
}

void decypherec(uint32_t dtbval, char *hash)
{
    uint8_t c = 0;
    uint32_t i = 0;
    uint64_t count = 1;
    puts("Enter pflash decypher key : \0");
    wait_unbusy();
    while (c != '\r')
    {
        c = getc();
        if (c == 127)
        {
            if (count == 1)
                continue;
            else
            {
                puts("\b \b");
                i--;
                cmd[i] = 0;
                count--;
            }
        }
        else
        {
            count++;
            putc(c);
            cmd[i] = c;
            i++;
        }
    }
    cmd[i] = 0;
    putc('\n');
    puts("Decypher in progress... \0");

    unsigned char hash2[65] = {0};
    char final[65];
    struct ctx hash_value;
    sha256_init(&hash_value);
    sha256_update(&hash_value, (unsigned char *) cmd, 16);
    sha256_final(&hash_value, hash2);
    ascii_to_hex(&final, hash2, 65);

    final[64] = 0;
    char final2[65] = {0};
    rev(&final2, (unsigned char *) final, 64);

    if(!strcmp((char *) hash, (char *) final2))
    {
        clear(size);
        puts("WRONG PASSWORD ! Retry : \n\0");
        decypherec(dtbval, hash);
    }
    else
    {
        aes_interface(magic->magic, 0x40080270, 0x400804fc, 1);  //kmain
        aes_interface(magic->magic, 0x400833f0, 0x400834fc, 1);  //help
        aes_interface(magic->magic, 0x40083530, 0x4008363c, 1);  //memdump
        aes_interface(magic->magic, 0x40083720, 0x400839ac, 1);  //memcheck

        puts("DONE !\n");
        kmain(dtbval);
    }
}

void Decypher(uint32_t dtbval)
{
    magic->magic[64] = 0;
    uint8_t c = 0;
    uint32_t i = 0;
    uint64_t count = 1;
    puts("Enter pflash decypher key : \0");
    wait_unbusy();
    while (c != '\r')
    {
        c = getc();
        if (c == 127)
        {
            if (count == 1)
                continue;
            else
            {
                puts("\b \b");
                i--;
                cmd[i] = 0;
                count--;
            }
        }
        else
        {
            count++;
            putc(c);
            cmd[i] = c;
            i++;
        }
        cmd[i] = 0;
    }
    putc('\n');
    puts("Decypher in progress... \0");

    unsigned char hash2[65] = {0};
    char final[65] = {0};

    struct ctx hash_value;
    sha256_init(&hash_value);
    sha256_update(&hash_value, (unsigned char *) cmd, 16);
    sha256_final(&hash_value, hash2);
    ascii_to_hex(&final, hash2, 65);

    final[64] = 0;
    char final2[65] = {0};
    rev(&final2, (unsigned char *) final, 64);
 
    if(!strcmp(magic->magic, final2))
    {
        puts("WRONG PASSWORD ! Retry :\n");
        decypherec(dtbval, magic->magic);
    }
    else
    {
        magic->magic[16] = 0;
        aes_interface(magic->magic, 0x40080270, 0x400804fc, 1);  //kmain
        aes_interface(magic->magic, 0x400833f0, 0x400834fc, 1);  //help
        aes_interface(magic->magic, 0x40083530, 0x4008363c, 1);  //memdump
        aes_interface(magic->magic, 0x40083720, 0x400839ac, 1);  //memcheck

        puts("DONE !\n");
        kmain(dtbval);
    }
}
