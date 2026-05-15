#include <stdio.h>
#include <stdlib.h> // Para system("pause")
#include <math.h>   // Para a função sin() e M_PI

// Se M_PI não for definido, podemos defini-lo manualmente
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Funções Auxiliares (adaptadas para não imprimir cabeçalhos internos) ---

// Gera a rampa de subida
// Retorna o número de passos gerados para a rampa de subida
int gerarRampaSubida(float offset, float dt_step, int n_step, int start_step) {
    float valor_atual = 0.0;
    int contador_passos = start_step; // Começa a contagem de passos a partir de 'start_step'
    int degraus_gerados = 0;

    int num_degraus_totais = (dt_step != 0) ? (int)(offset / dt_step) : 0; // Evita divisão por zero

    printf("\n--- Inicio Rampa de Subida ---\n");

    // Imprime o ponto inicial (0,0)
    printf("%d\t%.2f\n", contador_passos, 0.0f);
    contador_passos++;

    while (degraus_gerados < num_degraus_totais) {
        if ((contador_passos - start_step) % n_step == 0) { // Incrementa a cada n_step a partir do inicio desta fase
            valor_atual += dt_step;
            degraus_gerados++;
        }
        if (valor_atual > offset) { // Garante que não passe do offset
            valor_atual = offset;
        }
        
        printf("%d\t%.2f\n", contador_passos, valor_atual);
        if (valor_atual >= offset) break; // Sai se já atingiu o offset
        contador_passos++;
    }
    // Garante que o último ponto seja o offset, caso não tenha sido impresso no loop
    if (valor_atual < offset) {
         printf("%d\t%.2f\n", contador_passos, offset);
         contador_passos++;
    }
    printf("--- Fim Rampa de Subida ---\n");
    return contador_passos; // Retorna o próximo passo disponível
}

// Gera a senoide
// Retorna o número de passos gerados para a senoide
int gerarSenoide(float offset, float dt_amp, int n_period, int nb_period, int start_step) {
    int total_points = n_period * nb_period;
    float angulo_passo_radianos = (2.0 * M_PI) / n_period;
    int contador_passos = start_step;

    printf("\n--- Inicio Senoide ---\n");
    for (int i = 0; i < total_points; i++) {
        float angulo_radianos = i * angulo_passo_radianos;
        float valor_senoide = offset + dt_amp * sin(angulo_radianos);
        printf("%d\t%.2f\n", contador_passos, valor_senoide);
        contador_passos++;
    }
    printf("--- Fim Senoide ---\n");
    return contador_passos; // Retorna o próximo passo disponível
}

// Gera a rampa de descida
// Retorna o número de passos gerados para a rampa de descida
int gerarRampaDescida(float offset, float dt_step, int n_step, int start_step) {
    float valor_atual = offset; // Começa do valor do offset
    int contador_passos = start_step;
    int degraus_gerados = 0;

    int num_degraus_totais = (dt_step != 0) ? (int)(offset / dt_step) : 0; // Evita divisão por zero

    printf("\n--- Inicio Rampa de Descida ---\n");

    // Imprime o ponto inicial (Offset) se não foi o último ponto da fase anterior
    // (A rampa de subida já deve ter terminado no offset)
    // printf("%d\t%.2f\n", contador_passos, offset); // Não imprimir para evitar duplicidade, a senoide ou rampa anterior já terminou no offset

    while (degraus_gerados < num_degraus_totais) {
        if ((contador_passos - start_step) % n_step == 0 && (contador_passos - start_step) != 0) { // Decrementa a cada n_step
            valor_atual -= dt_step;
            degraus_gerados++;
        }
        if (valor_atual < 0) { // Garante que não passe de 0 para baixo
            valor_atual = 0;
        }

        printf("%d\t%.2f\n", contador_passos, valor_atual);
        if (valor_atual <= 0) break; // Sai se já atingiu 0 ou menos
        contador_passos++;
    }
    // Garante que o último ponto seja 0, caso não tenha sido impresso no loop
    if (valor_atual > 0) {
        printf("%d\t%.2f\n", contador_passos, 0.0f);
        contador_passos++;
    }
    printf("--- Fim Rampa de Descida ---\n");
    return contador_passos; // Retorna o próximo passo disponível
}


int main() {
    float offset_input;
    float dt_step_input; // Usado para rampa de subida e descida
    int n_step_input;    // Usado para rampa de subida e descida
    float dt_amp_input;  // Usado para senoide
    int n_period_input;  // Usado para senoide
    int nb_period_input; // Usado para senoide

    printf("--- Gerador de Sinal Integrado (Rampa Subida -> Senoide -> Rampa Descida) ---\n");

    // Coleta todos os inputs necessários
    printf("\n--- Configuracoes Gerais ---\n");
    printf("Digite o valor do Offset (para rampa final/inicial e centro da senoide): ");
    scanf("%f", &offset_input);

    printf("Digite o dt_step (incremento/decremento das rampas): ");
    scanf("%f", &dt_step_input);

    printf("Digite o n_step (passos para incremento/decremento das rampas): ");
    scanf("%d", &n_step_input);

    printf("\n--- Configuracoes da Senoide ---\n");
    printf("Digite o dt_amp (amplitude da senoide): ");
    scanf("%f", &dt_amp_input);

    printf("Digite o n_period (pontos de amostragem em 360 graus da senoide): ");
    scanf("%d", &n_period_input);

    printf("Digite o nb_period (quantidade de periodos da senoide): ");
    scanf("%d", &nb_period_input);

    // --- Validações ---
    if (offset_input < 0) {
        printf("Erro: Offset deve ser um valor positivo para esta sequencia de sinal.\n");
        return 1;
    }
    if (dt_step_input <= 0) {
        printf("Erro: dt_step deve ser um valor positivo para as rampas.\n");
        return 1;
    }
    if (n_step_input <= 0) {
        printf("Erro: n_step deve ser um valor inteiro positivo para as rampas.\n");
        return 1;
    }
    if (dt_amp_input < 0) { // Amplitude pode ser 0, mas não negativa para a interpretação padrão
        printf("Erro: dt_amp (amplitude) nao pode ser negativo.\n");
        return 1;
    }
    if (n_period_input <= 0) {
        printf("Erro: n_period deve ser um valor inteiro positivo para a senoide.\n");
        return 1;
    }
    if (nb_period_input <= 0) {
        printf("Erro: nb_period deve ser um valor inteiro positivo para a senoide.\n");
        return 1;
    }

    printf("\n\n--- Geracao do Sinal Combinado ---\n");
    printf("Passo\tValor do Sinal\n");
    printf("-----\t--------------\n");

    int current_step = 0; // Contador de passos global para todo o sinal

    // 1. Rampa de Subida
    current_step = gerarRampaSubida(offset_input, dt_step_input, n_step_input, current_step);

    // 2. Senoide
    // Se a rampa terminou antes do ultimo offset, a senoide deve começar no offset correto.
    // current_step já está com o valor do próximo passo.
    current_step = gerarSenoide(offset_input, dt_amp_input, n_period_input, nb_period_input, current_step);

    // 3. Rampa de Descida
    // A rampa de descida vai começar do valor do offset.
    current_step = gerarRampaDescida(offset_input, dt_step_input, n_step_input, current_step);

    printf("\n");
    #ifdef _WIN32
        system("pause"); 
    #endif

    return 0;
}
