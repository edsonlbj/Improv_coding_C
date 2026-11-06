#include <stdio.h>

int square(int x)
{
    return x * x;
}

int main()
{
    int x;
    printf("Digite o numero que deseja obter o quadrado: ");
    scanf("%d", &x); // Pay attention to not put spaces after the %d in a scanf command
    printf("O quadrado de %d é %d.\n", x, square(x));
    return 0;
}