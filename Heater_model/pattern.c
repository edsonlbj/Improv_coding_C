#include <stdio.h>
#include <stdlib.h> // Para system("pause")
#include <math.h>   // Para a função sin() e M_PI (constante pi)

// Se M_PI não for definido, podemos defini-lo manualmente
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void gerarSenoide(float offset, float dt_amp, int n_period, int nb_period) {
    printf("Gerando Senoide com:\n");
    printf("  Offset: %.2f\n", offset);
    printf("  Amplitude (dt_amp): %.2f\n", dt_amp);
    printf("  Pontos por Periodo (n_period): %d\n", n_period);
    printf("  Numero de Periodos (nb_period): %d\n", nb_period);
    printf("\n");

    // Calcular o número total de pontos a serem gerados
    int total_points = n_period * nb_period;

    // Calcular o "passo" em radianos para cada ponto
    // Um período completo é 2 * PI radianos
    // Então, cada ponto representa (2 * PI) / n_period radianos
    float angulo_passo_radianos = (2.0 * M_PI) / n_period;

    printf("Ponto\tAngulo (Graus)\tAngulo (Radianos)\tValor da Senoide\n");
    printf("-----\t--------------\t-----------------\t-----------------\n");

    for (int i = 0; i < total_points; i++) {
        // Calcular o ângulo atual em radianos
        float angulo_radianos = i * angulo_passo_radianos;
        
        // Calcular o valor da função seno
        // Formula: valor_final = Offset + Amplitude * sin(angulo)
        float valor_senoide = offset + dt_amp * sin(angulo_radianos);

        // Opcional: Calcular o ângulo em graus para melhor visualização
        float angulo_graus = (angulo_radianos * 180.0) / M_PI;

        printf("%d\t%.2f\t\t%.4f\t\t\t%.2f\n", i, angulo_graus, angulo_radianos, valor_senoide);
    }
}

int main() {
    float offset_input;
    float dt_amp_input;
    int n_period_input;
    int nb_period_input;

    printf("--- Gerador de Funcao Seno ---\n");

    printf("Digite o valor do Offset (valor central da senoide): ");
    scanf("%f", &offset_input);

    printf("Digite a Amplitude (dt_amp - valor maximo atingido acima/abaixo do Offset): ");
    scanf("%f", &dt_amp_input);

    printf("Digite o numero de pontos de amostragem por periodo (n_period): ");
    scanf("%d", &n_period_input);

    printf("Digite a quantidade de periodos a serem gerados (nb_period): ");
    scanf("%d", &nb_period_input);

    // Validações
    if (n_period_input <= 0) {
        printf("Erro: n_period deve ser um valor inteiro positivo (numero de pontos por periodo).\n");
        return 1;
    }
    if (nb_period_input <= 0) {
        printf("Erro: nb_period deve ser um valor inteiro positivo (numero de periodos).\n");
        return 1;
    }

    gerarSenoide(offset_input, dt_amp_input, n_period_input, nb_period_input);

    printf("\n");
    #ifdef _WIN32
        system("pause"); 
    #endif

    return 0;
}
