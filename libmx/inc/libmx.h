#ifndef LIBMX_H
#define LIBMX_H
#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <unistd.h>
#include <wchar.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <crypt.h>
#if defined(__APPLE__)
    #include <malloc/malloc.h>
    #define malloc_size malloc_size
#elif defined(_WIN64) || defined(_WIN32)
    #include <malloc.h>
    #define malloc_size _msize
#elif defined(__linux__)
    #include <malloc.h>
    #define malloc_size malloc_usable_size
#endif

typedef struct s_list {
    void *data;
    struct s_list *next;
} t_list;

void mx_insert_value(unsigned int **arr, unsigned int insert_value);
bool mx_is_in_array(unsigned int *arr, unsigned int value);
unsigned int b64_decodef(char *InFile, char *OutFile);
unsigned int b64_encodef(char *InFile, char *OutFile);
unsigned int b64_decode(const unsigned char* in, unsigned int in_len, unsigned char* out);
unsigned int b64_encode(const unsigned char* in, unsigned int in_len, unsigned char* out);
unsigned int b64d_size(unsigned int in_size);
unsigned int b64e_size(unsigned int in_size);
unsigned int b64_int(unsigned int ch);
void mx_remove_symbol(char *str, char symbol);
int mx_str_contains(const char *str, char target_letter);
char* mx_replace_sub_string(char* original, const char* substring, const char* replacement);
int mx_get_length(char **arr);
char* mx_ultoa(unsigned long value);
char *mx_double_to_str(double number);
char *mx_lltoa(long long n);
int mx_intlen(int n);
char* mx_char_to_str(const char source[]);
int mx_get_index(char **strarr, char *str);
long long mx_atoll(const char *str);
bool mx_isalpha(int c);
void mx_printerr(const char *s);
int mx_sscanf(const char *str, const char *format, char *flag, char *directory);
int mx_atoi(const char *str);
void mx_printchar(const char c);
void mx_swap_char(char *s1, char *s2);
int mx_get_char_index(const char *str, char c);
char *mx_strnew(const int size);
char *mx_strdup(const char *str);
char *mx_strtok(char *str, const char *delim);
char* mx_strchr(const char* str, int c);
char *mx_nbr_to_hex(unsigned long nbr);
char *mx_strcpy(char *dst, const char *src);
void mx_swap_str(char **s1, char **s2);
int mx_isprint(char c);
double mx_pow(double n, unsigned int pow);
int mx_isspace(char c);
unsigned long mx_hex_to_nbr(const char *hex);
int mx_sqrt(int x);
int mx_strlen(const char *s);
int mx_bubble_sort(char **arr, int size);
int mx_strcmp(const char *s1, const char *s2);
int mx_binary_search(char **arr, int size, const char *s, int *count);
void mx_printint(int n);
void mx_printchar(char c);
void mx_printstr(const char *s);
void mx_print_unicode(wchar_t c);
void mx_foreach(int *arr, int size, void (*f)(int));
void mx_print_strarr(char **arr, const char *delim);
void mx_str_reverse(char *s);
void mx_strdel(char **str);
void mx_del_strarr(char ***arr);
int mx_quicksort(char **arr, int left, int right);
char *mx_itoa(int number);
char *mx_strndup(const char *s1, size_t n);
char *mx_strncpy(char *dst, const char *src, int len);
char *mx_strcat(char *restrict s1, const char *restrict s2);
char *mx_strstr(const char *haystack, const char *needle);
int mx_get_substr_index(const char *str, const char *sub);
int mx_count_substr(const char *str, const char *sub);
int mx_count_words(const char *str, char c);
char *mx_strtrim(const char *str);
char *mx_del_extra_spaces(const char *str);
char **mx_strsplit(const char *s, char c);
char *mx_strjoin(const char *s1, const char *s2);
char *mx_file_to_str(const char *file);
int mx_read_line(char **lineptr, size_t buf_size, char delim, const int fd);
char *mx_replace_substr(const char *str, const char *sub, const char *replace);
int mx_strncmp(const char *s1, const char *s2, int n);
char *mx_strncat(char *restrict s1, const char *restrict s2, size_t n);
void *mx_memset(void *b, int c, size_t len);
void *mx_memcpy(void *restrict dst, const void *restrict src, size_t n);
void *mx_memccpy(void *restrict dst, const void *restrict src, int c, size_t n);
int mx_memcmp(const void *s1, const void *s2, size_t n);
void *mx_memchr(const void *s, int c, size_t n);
void *mx_memrchr(const void *s, int c, size_t n);
void *mx_memmem(const void *big, size_t big_len, const void *little, size_t little_len);
void *mx_memmove(void *dst, const void *src, size_t len);
void *mx_realloc(void *ptr, size_t size);

bool mx_isdigit(int c);
t_list *mx_create_node(void *data);
void mx_push_front(t_list **list, void *data);
void mx_push_back(t_list **list, void *data);
void mx_pop_front(t_list **head);
void mx_pop_back(t_list **head);
int mx_list_size(t_list *list);
t_list *mx_sort_list(t_list *lst, bool (*cmp)(void *, void *));

#endif
