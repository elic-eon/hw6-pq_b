#include<stdio.h>

int main()
{
    int n;
    scanf("%d", &n);
    printf("%c %c %d\n", 's', 's', n);
    int i;
    for (i = 1; i <= n; i++)
    {
        printf("%d %d %d\n", 1, (i%2) ? (i+1)/2 : n - (i-1)/2, i);
    }
    for ( i = 1; i <= n; i++)
    {
        printf("%d%s", (i%2) ? 2:3, (i%10)? " ":"\n");
    }
    printf("10\n");
}
