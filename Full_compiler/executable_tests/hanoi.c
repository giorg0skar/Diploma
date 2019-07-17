#include <stdio.h>

void move(char *source, char *target) {
    printf("Moving from %s to %s.\n", source, target);
}

void hanoi(int rings, char *source, char *target, char *auxiliary) {
    if (rings >= 1) {
        hanoi(rings-1, source, auxiliary, target);
        move(source, target);
        hanoi(rings-1, auxiliary, target, source);
    }
}

int main() {
    int numberOfrings;
    scanf("%d", &numberOfrings);
    char l[] = "left";
    char r[] = "right";
    char m[] = "middle";
    printf("Rings: ");
    hanoi(numberOfrings, l, r, m);
    return 0;
}