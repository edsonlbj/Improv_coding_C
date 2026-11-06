#include "stdio.h"

int sum (int x, int y) 
{
    return x + y;
}

int main()
{
    int x, y;
    printf("Digite o primeiro numero da soma: ");
    scanf("%d", &x); // Pay attention to not put spaces after the %d in a scanf command
    printf("\n\nDigite o segundo numero da soma: ");
    scanf("%d", &y);
    printf("\n\nA soma de %d + %d = %d\n", x, y, sum(x, y));
    return(0);
} 