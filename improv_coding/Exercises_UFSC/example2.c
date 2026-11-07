#include <limits.h>
#include <stdio.h>

int sum(int x, int y)
{
    return x + y;
}

int main(void)
{
    long long x_longo;
    long long y_longo;
    int x, y;

    printf("Digite o primeiro numero da soma: ");
    if (scanf("%lld", &x_longo) != 1) {
        printf("Entrada invalida.\n");
        return 1;
    }

    if (x_longo < INT_MIN || x_longo > INT_MAX) {
        printf("Valor invalido para x. Digite um inteiro dentro do intervalo [%d, %d].\n", INT_MIN, INT_MAX);
        return 1;
    }

    x = (int)x_longo;

    printf("\nDigite o segundo numero da soma: ");
    if (scanf("%lld", &y_longo) != 1) {
        printf("Entrada invalida.\n");
        return 1;
    }

    if (y_longo < INT_MIN || y_longo > INT_MAX) {
        printf("Valor invalido para y. Digite um inteiro dentro do intervalo [%d, %d].\n", INT_MIN, INT_MAX);
        return 1;
    }

    y = (int)y_longo;
    printf("\nA soma de %d + %d = %d\n", x, y, sum(x, y));

    return 0;
} 