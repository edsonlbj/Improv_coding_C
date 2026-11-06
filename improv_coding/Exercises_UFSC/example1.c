#include "stdio.h"
void main()
{
    int Dias;
    float Anos;
    printf("Entre com o numero de dias que deseja converter em anos: "); // Data input
    scanf("%d", &Dias);
    Anos = Dias/365.25;
    printf("\n\n %d dias equivalem a %f anos. \n", Dias, Anos);
}