#include <stdint.h>
#include <stdio.h>

uint64_t strlen(char *str)
{
    uint64_t count = 0;
    while (str[count] != 0)
        count++;
    return count;
}

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


uint64_t get_8bits(char *str, uint64_t index)
{
    uint64_t val = hextoi(str + index);
    for(uint8_t i = 8; i > 0; i--)
    {
        str[index] = 0;
        index++;
    }
    return val;
}

void init_tab(char (*tab)[], char * str, uint64_t size)
{
    uint64_t i = 0;
    while(size > 0)
    {
        (*tab)[i] = str[i];
        size--;
        i++;
    }
}

uint8_t main()
{
    char *test1 = "b3525698549eb6c98868c4d7b227b9fdc4051f224aa9d21320feda96dac840d43c6c751659eddc635bc7e254e4d697f9d8f1e8dbbf3dc2267f411d63a8c157ae7541b912f00e4198459a1973ff900379d3fa8a6094bc445c904ed6f51e715cc05748003b389919596f5c29ec5295928555d30c6900e4cfdd4e1e007233b834cf";
    char test[256];
    init_tab(&test, test1, 256);
    uint64_t i = strlen(test) - 8;

    uint64_t first = get_8bits(test, i);
    i -= 8;
    printf("%ld\n", first);
    uint64_t second = get_8bits(test, i);
    printf("%ld\n", second);
    i -= 8;
    uint64_t third = get_8bits(test, i);
    printf("%ld\n", third);

}