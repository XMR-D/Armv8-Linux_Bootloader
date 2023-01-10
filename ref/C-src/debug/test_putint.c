#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void putint(uint64_t nb)
{
    uint64_t count = 1;
    uint64_t saved = nb;
    if(nb == 0)
        printf("0\n");
    else
    {
        while(nb >= 10)
        {
            nb /= 10;
            count *= 10;
        }
        while(count > 0)
        {
            printf("%c", (char) ((saved/count) + 48));
            saved -= (saved/count)*count;
            count /= 10;
        }
    }
    printf("\n");
}

int main()
{
    putint(0);
    putint(485645465841);
    putint(1073741824);
    system("echo toto");
}
