#include "stdio.h"

int sum (int x, int y) 
{
    return x + y;
}

int main()
{
    int x, y;
    printf("Digite o primeiro numero da soma: ");
    scanf("%d", &x); // Note: spaces after %d in the format string are ignored; spaces before %d can affect input parsing
    printf("\nDigite o segundo numero da soma: ");
    scanf("%d", &y);
    printf("\nA soma de %d + %d = %d\n", x, y, sum(x, y));
    return(0);
} 