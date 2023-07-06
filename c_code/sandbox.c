#include <stdio.h>
#include <stdlib.h>

int foo(const void *, const void *);

int main() {
    int a[] = {2, 5, 10, 59, 200, 0, 23};
    int length = 7;

    qsort(a, length, sizeof(int), foo);

    return 0;
}

int foo(const void *x, const void *y) {
    int x_val = *(int *)x;
    int y_val = *(int *)y;
    
    return x_val - y_val;
}
