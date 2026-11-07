#include <limits.h>
#include <stdio.h>

int main(void)
{
    long long dias_longo;
    int dias;
    double anos;

    printf("Entre com o numero de dias que deseja converter em anos: ");
    if (scanf("%lld", &dias_longo) != 1) {
        printf("Entrada invalida.\n");
        return 1;
    }

    if (dias_longo < 0 || dias_longo > INT_MAX) {
        printf("Valor invalido. Digite um numero inteiro positivo menor ou igual a %d.\n", INT_MAX);
        return 1;
    }

    dias = (int) dias_longo;
    anos = dias / 365.25;
    printf("\n%d dias equivalem a %.2f anos.\n", dias, anos);

    return 0;
}