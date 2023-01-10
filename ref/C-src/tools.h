#ifndef TOOLS_H
#define TOOLS_H

//commands parser functions
void clear(uint32_t size);
void parsing_cmd();
void dump_parsing(void);

//math operations functions
uint64_t pow(uint64_t val, uint64_t exp);
uint64_t truncated(uint64_t val);

//strings operations functions
uint32_t strcmp(char *str1, char *str2);
uint64_t strlen(char *str);
char *to_lower(char *to_lower);
uint64_t my_atoi(char *to_parse);
uint64_t hextoi(char *to_parse);

//chars operation functions
uint8_t is_printable(char a);

//memory operations functions
uint64_t dump_content_at(uint64_t start_offset, uint8_t load_range);
uint8_t checkboard(uint8_t config, uint8_t *ptr, uint8_t g);
uint8_t check_mem_at(uint64_t offset);

void generate_report(uint64_t *tab, uint8_t mod, uint64_t size);
#endif /* TOOL_H */
