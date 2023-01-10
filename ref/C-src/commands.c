#include <stddef.h>
#include "serial.h"
#include "commands.h"
#include "tools.h"
extern char *args[5];
uint64_t writeable = 0;

//HELP routine, display help and command available with a short description
void help(void)
{
    putc('\n');
    puts("Number of commands available : 5\n");
    putc('\n');
    puts("  boot : boot\n\n");
    puts("       Launch my_pikaboot kernel.\n\n");
    puts("  eboot : eboot\n\n");
    puts("       Place the bootloader in listening mode, in order to emergency boot,\n");
    puts("       to make it functionnal you need to execute the Emergency_boot.py\n");
    puts("       script in a shared environnement with a char device.\n");
    putc('\n');
    puts("  md : md [start_address] [range] [load_of_bytes]\n\n");
    puts("       Dump \"load_nb bytes\" of the memory starting at \"start_address\",\n");
    puts("       within a range of \"range\".\n\n");
    puts("       start_address : 64-bit hexadecimal unsigned int.\n");
    puts("       range : 64-bit unsigned int.\n");
    puts("       load_of_bytes : 64-bit unsigned int.\n");
    putc('\n');
    puts("  memtest : memtest [granularity] [start_address] [end_address]\n\n");
    puts("       test the cells of the memory starting at \"start_address\",\n");
    puts("       to \"end_address\", with a granularity of \"granularity\".\n\n");
    puts("       granularity : 64-bit unsigned int.\n");
    puts("       start_address : 64-bit hexadecimal unsigned int.\n");
    puts("       end_address : 64-bit hexadecimal unsigned int.\n");
    putc('\n');
    puts("  help : help\n\n");
    puts("       Display this help.\n");
    putc('\n');
}

//MEMORY DUMP routine, dump the memory and print an ascii representation of the memory dumped
// argc ===> nb of arguments passed to the routine
void mem_dump(uint8_t argc)
{
    if (argc != 4)
    {
        puts("md: Error. Wrong number of argument (see help)\n");
        puts("Usage: md [start_address] [range] [load_of_bytes]\n\n");
        return;
    }

    args[3][strlen(args[3]) - 1] = 0; //remove last char of last argument

    uint64_t start_offset = hextoi(args[1]); //get addr
    uint64_t address_range = my_atoi(args[2]); //get range
    start_offset = truncated(start_offset); //truncate addr to alignement

    uint64_t load_nb = my_atoi(args[3]);
    uint8_t c = 0;
    uint64_t limit = start_offset + address_range;

    if ((load_nb != 1 && (load_nb % 2 != 0)) || load_nb > 8)
    {
        puts("md: Error. Load number invalid, expected 1, 2, 4 or 8\n");
        return;
    }

    while(start_offset < limit)
    {
        puts("0x");
        if (c == 0)
        {
            puthex(start_offset, 0);
            c++;
        }
        else
        {
            puthex(start_offset, 0);
        }
        putc(' ');
        start_offset = dump_content_at(start_offset, load_nb);
    }
    return;
}

//MEMORY TEST routine, test memory cells to see if writable or readable
// gran ===> byte granularity chosen
// argc ===> nb of arguments passed to the routine
void mem_test(char *gran, uint8_t argc)
{

    args[3][strlen(args[3]) - 1] = 0; //remove /r of last argument
    uint8_t g = my_atoi(gran);

    if (argc != 4)
    {
        puts("memtest: Error. Wrong number of argument (see help)\n");
        puts("Usage: memtest [granularity] [start_address] [end_address]\n\n");
        return;
    }

    if ((g != 1 && (g % 2 != 0)) || g > 8)
    {
        puts("memtest: Error. granularity number invalid, expected 1, 2, 4 or 8\n");
        return;
    }

    puts("=====================================================\n");
    puts("|_____________MemTest routine report :______________|\n\n");
    uint64_t actual_offset = hextoi(args[2]);
    uint64_t end_offset = hextoi(args[3]);

    uint64_t mod_progress = (end_offset - actual_offset) / 100;
    uint8_t error_check = 0; //memcheck return value
    uint8_t i = 0; //index in tab
    uint64_t tab[4096]; //table of address where a change of error is detected
    uint8_t report = 1; //int to skip address that provoke no change
    uint8_t saved = 0;
    uint8_t progress = 0; //progress percent
    uint8_t mod = 0;
    uint64_t count = 0;
    uint64_t size = 0;

    while (actual_offset <= end_offset)
    {
        error_check = check_mem_at(actual_offset);
        saved = error_check;

        if (count == mod_progress)
        {
            count = 0;
            progress += g;
        }
        if (actual_offset == end_offset)
            progress += (100 - progress);

        puts("Progress : ");
        putint(progress);
        puts("% Actual_offset = ");
        puthex(actual_offset, 0);
        putc('\r');
        count++;

        if (error_check != saved) //swap of type detected activate report mode
            report++;

        if (i == 0 && error_check == 1) //get param to decide order in report generation
            mod = 1;

        if (report)
        {
            tab[i++] = actual_offset;
            size++;
            report = 0;
        }
        actual_offset += g;
    }
    tab[i] = actual_offset - g;
    size++;
    generate_report(tab, mod, size);
    puts("\n=====================================================\n");
}
