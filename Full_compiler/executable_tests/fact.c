#include <stdio.h>

int fact(int n) {
    if (n <= 1) return 1;
    else return n * fact(n-1);
}

int main() {
    int i,f;
    scanf("%d", &i);
    if (i >= 0) printf("%d\n", fact(i));
    else printf("Give a non negative number\n");
    return 0;
}