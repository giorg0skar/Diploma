#include <stdio.h>
#include <string.h>

char r[32];

void reverse(char s[]) {
    int len = strlen(s);
    int i;
    for(i=0; i<len; i++) {
        r[i] = s[len-i-1];
    }
    r[i] = '\0';
}

int main() {
    //char str = {'\n', '!', 'd', 'o','l', 'l', 'e', 'h'};
    //char sentence[] = "!dlrow olleH";
    char sentence[5];
    scanf("%s", sentence);
    reverse(sentence);
    // for(int i=0; i<strlen(sentence); i++) {
    //     printf("%c", r[i]);
    // }
    printf("%s\n", r);
    return 0;
}