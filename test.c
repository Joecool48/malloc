#include"mymalloc.h"
#include<stdio.h>
int main() {
    int * arr = (int*) mymalloc(30 * sizeof(int));
    for(int i = 0; i < 30; i++) {
        arr[i] = i;
    }
    for (int i = 0; i < 30; i++) {
        printf("%d ", arr[i]);
    }
}
