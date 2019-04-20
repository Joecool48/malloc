#include"mymalloc.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define MAX_STRING_SIZE 700 
#define MAX_STRUCTS 200

typedef struct {
    char * mem;
} malloc_test;

typedef struct {
    size_t size;
    uint8_t isUsed;
    char str[MAX_STRING_SIZE];
} static_test;

malloc_test malloc_structs[MAX_STRUCTS];
static_test static_structs[MAX_STRUCTS];

void passed() {
    printf( GREEN "Tests passed!\n" RESET);
}

void failed(int num) {
    printf( RED "Tests failed at %d\n" RESET, num);
}

void create_random_string(char * buf, size_t max_size) {
    for (int i = 0; i < max_size - 1; i++) {
        buf[i] = 'a' + rand() % 26;
    }
    buf[max_size - 1] = 0; // null terminate
}

void do_mallocs() {
    // loop through, and if free malloc it
    char string_buf[MAX_STRING_SIZE];
    for (int i = 0; i < MAX_STRUCTS; i++) {
        if(!static_structs[i].isUsed) {
            size_t str_size = rand() % (MAX_STRING_SIZE) + 1;
            create_random_string(string_buf, str_size);
            memcpy(static_structs[i].str, string_buf, str_size);
            static_structs[i].isUsed = 1;
            static_structs[i].size = str_size;
            malloc_structs[i].mem = (char *) mymalloc(str_size);
            memcpy(malloc_structs[i].mem, string_buf, str_size);
        }
    } 
}

void do_frees() {
    for (int i = 0; i < MAX_STRUCTS; i++) {
        if (rand() % 2) {
            static_structs[i].isUsed = 0;
            myfree(malloc_structs[i].mem);
        }
    }
}

void check() {
    for (int i = 0; i < MAX_STRUCTS; i++) {
        if (static_structs[i].isUsed) {
            // make sure it has right contents
            printf("malloc: %s\n", malloc_structs[i].mem);
            printf("static: %s\n", static_structs[i].str);
            if (strcmp(malloc_structs[i].mem, static_structs[i].str)) {

                failed(i);
                return;
            }
        }
    }
    passed();
}

void reset() {
    srand(time(NULL));
    memset(malloc_structs, 0, sizeof(malloc_test) * MAX_STRUCTS);
    memset(static_structs, 0, sizeof(static_test) * MAX_STRUCTS); 
     
}

void do_tests() {
    reset();
    do_mallocs();
    do_frees();
    do_mallocs();
    do_frees();
    do_mallocs();
    do_frees();
    check(); 
}

int main() {
    do_tests(); 
}
