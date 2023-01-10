#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint64_t writable;
char s[64192] = {0};

void lputc(char c)
{
    printf("%c", c);
}

uint64_t atoi(char *to_parse)
{
    uint32_t count = 0;
    uint64_t slen = strlen(to_parse);
    for (uint64_t i = 0; i < slen; i++)
    {
        count = (count * 10) + to_parse[i] - '0';
    }
    return count;
}


void putint(uint64_t nb)
{
    int count = 1;
    int x = 0;
    int saved = nb;
    if(nb == 0)
        lputc('0');
    else
    {
        while(nb > 0)
        {
            nb /= 10;
            count *= 10;
        }
        while(count > 0)
        {
            if(x >= 1)
            {
                lputc((saved/count) + 48);
                saved -= (saved/count)*count;
                count /= 10;
            }
            else
            {
                x += 1;
                count /= 10;
            }
        }
    }
}

uint8_t checkboard(uint8_t config, uint8_t *ptr, uint8_t g)
{
    uint8_t mask1 = 0xAA;
    uint8_t mask2 = 0x55;
    if (config == 0)
    {
        mask1 = 0x55;
        mask2 = 0xAA;
    }
    while (g > 0)
    {
        if (g % 2 == 0)
        {
            if (*ptr != mask2)
                return 0;
        }
        else
        {
            if (*ptr != mask1)
                return 0;
        }
        ptr++;
        g--;
    }
    return 1;

}

uint8_t check_mem_at(uint64_t offset,  uint8_t g)
{
    uint8_t *ptr = (uint8_t *) &s;
    uint8_t saved = *ptr;
    uint8_t mask1 = 0xAA;
    uint8_t mask2 = 0x55;
    uint8_t count = g;
    if (g > 1) //Use checkboard algorithm
    {
        while (count > 0) //write AA55
        {
            if (count % 2 == 0)
                *ptr = mask1;
            else
                *ptr = mask2;
            ptr++;
            count--;
        }
        ptr = (uint8_t *) &s;
        if(!checkboard(0, ptr, g))
            return 1;
        count = g;
        while (count > 0)
        {
            if (count % 2 == 0)
                *ptr = mask2;
            else
                *ptr = mask1;
            ptr++;
            count--;
        }
        ptr = (uint8_t *) &s;
        if(!checkboard(1, ptr, g))
            return 1;
        *ptr = saved;
        writable++;
    }
    else //Use MSCAN algorithm
    {
        *ptr = 0;//write 0
        if (*ptr != 0) //read 0
            return 1;
        *ptr = 1; //write 1
        if (*ptr != 1) //read 1
            return 1;
        *ptr = saved;
        writable++;
    }
    return 0;
}


void mem_test(char *gran, uint8_t argc)
{

    uint8_t g = atoi(gran);
    if (argc != 2)
    {
        printf("memtest: Error. Wrong number of argument (see help)\n");
        return;
    }

    if ((g != 1 && (g % 2 != 0)) || g > 8)
    {
        printf("memtest: Error. granularity number invalid, expected 1, 2, 4 or 8\n");
        return;
    }

    printf("_______MemTest routine report :_______\n");
    uint64_t actual_offset = 0;
    uint8_t error_check = 0;
    uint8_t progress = 0;
    uint64_t count = (uint64_t) &s;
    while (count < ((uint64_t) &s + 64192))  //(actual_offset <= 0x7fff0000)
    {
        if (count % 2 == 0)
            progress++;
        error_check = check_mem_at(actual_offset, g);
        actual_offset += g;
        if (error_check)
        {
            printf("aborting.\n\n");
            printf("A unwriteable/readable cell has been encountered.\n\n");
            printf("Progress = ");
            putint(progress);
            printf("%%\n\n");
            return;
        }
        printf("Progress = ");
        putint(progress);
        printf("%%\r");
        count++;
    }

    printf("number of writable/readable cells : ");
    putint(writable);
    lputc('\n');
    printf("granularity chosen : ");
    printf("%s", gran);
    printf("\nNo problems encountered, all memcells are writeable/readable\n");
    printf("Status = 100%% memory checked\n");

}

int main()
{
    mem_test("2", 2);
}
