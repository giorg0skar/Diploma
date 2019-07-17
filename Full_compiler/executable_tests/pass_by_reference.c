#include <stdio.h>

int main() {
    int i;

    void gfunc(int *x) {
        *x = *x + 1;
        return;
    }
    i = 1;
    printf("Before: %d\n", i);
    gfunc(&i);
    printf("After: %d\n", i);
    return 0;
}