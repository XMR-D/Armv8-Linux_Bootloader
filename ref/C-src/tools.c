#include <stdint.h>
#include <stddef.h>
#include "serial.h"
#include "tools.h"

//get the buffer from kmain.c define a struct args
extern char cmd[512];

char *args[5];
char *null = "\0";

//clear cmd
void clear(uint32_t size)
{
    for(uint32_t i = 0; i < size; i++)
        cmd[i] = 0;
}

//compare cmd with a str
uint32_t strcmp(char *str1, char *str2)
{
    for(uint32_t i = 0; str2[i] != 0; i++)
    {
        if(str1[i] != str2[i])
            return 0;
    }
    return 1;
}

//print the content of args (the actual argument stored)
void dump_parsing(void)
{
    for (uint32_t i = 0; i < 5; i++)
    {
        puts(args[i]);
        putc('\n');
    }
}
void args_to_null(void)
{
    for(uint8_t i = 0; i < 5; i++)
        args[i] = null;
}
//parsing preprocessed cmd
//j is the index in the buffer with a step of 1
//i is the index in args table within arguments struct with a step of 1
//return the number of argument set in the structs
void parsing_cmd()
{
    uint32_t j = 0;
    uint32_t i = 0;
    uint8_t stop = 0;
    args_to_null();
    while (i < 5)
    {
        if (cmd[j] != 0 && stop == 0)
        {
            args[i] = &cmd[j];
            stop++;
            i++;
        }
        else if (cmd[j] == 0)
        {
            stop = 0;
        }
        else if (cmd[j] == '\r')
        {
            break;
        }
        j++;
    }
}

//compute val to the power of exp (val^exp)
uint64_t pow(uint64_t val, uint64_t exp)
{
    uint64_t count = 1;
    while (exp > 0)
    {
        count *= val;
        exp--;
    }
    return count;
}

//get the number of char composing str
uint64_t strlen(char *str)
{
    uint64_t count = 0;
    while (str[count] != 0)
        count++;
    return count;
}

//return a string containing all chars in str in lower case
char *to_lower(char *str)
{
    uint32_t i = 0;
    while (str[i] != 0)
    {
        if (str[i] >= 65 && str[i] <= 90)
            str[i] += 32;
        i++;
    }
    return str;
}

//parse to_parse string and return the value of his representation
// "0x133" = 307
// "0x1a464652" = 440813138
uint64_t hextoi(char *to_parse)
{
    uint64_t len = strlen(to_parse);
    uint64_t base = 1;
    uint64_t res = 0;
    for(int64_t i = len--; i >= 0; i--)
    {
        if(to_parse[i] >= '0' && to_parse[i] <= '9')
        {
            res += (to_parse[i] - 48) * base;
            base *= 16;
        }
        else if(to_parse[i] >= 'A' && to_parse[i] <= 'F')
        {
            res += (to_parse[i] - 55) * base;
            base *= 16;
        }
        else if(to_parse[i] >= 'a' && to_parse[i] <= 'f')
        {
            res += (to_parse[i] - 87) * base;
            base *= 16;
        }
    }
    return res;
}

//take a numeric string dec value ex : "15665" 
//and return his base 10 value
uint64_t my_atoi(char *to_parse)
{
    uint32_t count = 0;
    uint64_t slen = strlen(to_parse);
    for (uint64_t i = 0; i < slen; i++)
    {
        count = (count * 10) + to_parse[i] - '0';
    }
    return count;
}

//check if a is printable, "abcdefghijklmnopqrstuvwxyz,;:!ù^$&é"'(-è_çà....."
//a is not case sensitive
uint8_t is_printable(char a)
{
    return (a >= 33 && a <= 126);
}


//dump memory content at start_offset separting it following load_range
//load_range = 1 ===> 00 00 00 00 00...
//           = 2 ===> 0000 0000 0000 0000....
//           = 4 ===> 00000000 00000000 00000000.....
//           = 8 ===> 0000000000000000 0000000000000000.....
//print an ascii representation of the memory
uint64_t dump_content_at(uint64_t start_offset, uint8_t load_range)
{
    char buff[17] = {0};
    buff[17] = 0;
    uint8_t saved_load = load_range;
    uint8_t index = saved_load;

    for (uint8_t i = 1; i <= 16; i++)
    {

        if (index + load_range != saved_load)
        {
            index--;
        }
        else
        {
            saved_load += load_range;
            index = saved_load - 1;
            start_offset += load_range;

        }

        //get the memory at addr start_offset shifted by index % load_range
        uint8_t byteval_s = *((uint8_t *) start_offset + (index % load_range));

        if(is_printable(byteval_s))
            buff[index] = byteval_s;
        else
            buff[index] = '.';


        if (byteval_s == 0)
            puts("00");
        else
            puthex(byteval_s, 1);

        if (i % load_range == 0)
            putc(' ');

    }
    puts(" | ");
    puts(buff);
    putc('\n');
    return start_offset + load_range;
}

uint64_t truncated(uint64_t val)
{
    uint64_t mod = val % 16;
    if (mod != 0)
        val -= mod;
    return val;
}


//other possible algorithms : Galloping test, butterfly algorithm
uint8_t check_mem_at(uint64_t offset)
{
    int8_t *ptr = (int8_t *) offset;
    int8_t saved = *ptr;
    *ptr = ~(*ptr);

if (saved != ~(*ptr))
    {
        *ptr = ~(*ptr);
        return 1;
    }

    *ptr = ~(*ptr);
    return 0;
}

void generate_report(uint64_t *tab, uint8_t mod, uint64_t size)
{
    uint8_t index = 0;
    while (index < size)
    {
        if (mod && index % 2 == 0)
        {
            puts("\nUnwritable from offset : ");
            puthex(tab[index], 0);
            puts(" to ");
            puthex(tab[++index], 0);
        }
        else if (mod)
        {
            puts("\nWritable from offset : ");
            puthex(tab[index], 0);
            puts(" to ");
            puthex(tab[++index], 0);
        }
        else if (!mod && index % 2 != 0)
        {
            puts("\nUnwritable from offset : ");
            puthex(tab[index], 0);
            puts(" to ");
            puthex(tab[++index], 0);
        }
        else
        {
            puts("\nWritable from offset : ");
            puthex(tab[index], 0);
            puts(" to ");
            puthex(tab[++index], 0);
        }
        index++;
    }

}
