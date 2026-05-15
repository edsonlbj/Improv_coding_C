#include <stdio.h>
#include <stdlib.h> // Para a função system("pause") no Windows

void gerarRampaInvertida(float offset, float dt_step, int n_step) {
    // Calcula o número total de degraus para chegar a zero a partir do offset
    // Se dt_step for 0, evitar divisão por zero
    if (dt_step == 0) {
        printf("Erro: dt_step nao pode ser zero para uma rampa decrescente.\n");
        return;
    }
    
    // Para uma rampa decrescente, o dt_step deve ser considerado positivo para o cálculo
    // e aplicado como decremento.
    int num_degraus_totais = (int)(offset / dt_step);
    
    printf("Gerando rampa INVERTIDA com:\n");
    printf("  Offset: %.2f (valor inicial)\n", offset);
    printf("  dt_step: %.2f (valor de cada decremento)\n", dt_step);
    printf("  n_step: %d\n", n_step);
    printf("  Numero total de degraus para atingir zero: %d\n", num_degraus_totais);
    printf("\n");

    float valor_atual = offset; // Começa do valor do offset
    int contador_passos = 0;
    int degraus_gerados = 0;

    printf("Passo\tValor da Rampa\n");
    printf("-----\t--------------\n");

    // Loop para gerar a rampa até atingir ou ficar abaixo de zero
    while (valor_atual > 0 || degraus_gerados <= num_degraus_totais) { // Continua enquanto for maior que 0 ou houver degraus para gerar
        // Incrementa o valor da rampa a cada n_step passos
        if (contador_passos % n_step == 0 && contador_passos != 0) {
            valor_atual -= dt_step; // DECREMENTA o valor
            degraus_gerados++;
        }
        
        // Garante que o último valor não ultrapasse o zero para baixo (fique negativo demais)
        if (valor_atual < 0 && degraus_gerados > num_degraus_totais) {
            valor_atual = 0;
        }

        printf("%d\t%.2f\n", contador_passos, valor_atual);
        contador_passos++;

        // Condição de saída adicional para evitar loop infinito e parar em zero
        if (valor_atual <= 0 && degraus_gerados >= num_degraus_totais) {
            break; 
        }
    }
    
    // Se o loop terminou e o valor_atual ainda não é zero (devido a imprecisão de float)
    // ou se o último decremento o deixou acima de zero mas era para ser zero.
    if (valor_atual > 0) {
        valor_atual = 0;
        printf("%d\t%.2f\n", contador_passos, valor_atual);
    }
}

int main() {
    float offset_input;
    float dt_step_input;
    int n_step_input;

    printf("--- Gerador de Rampa INVERTIDA ---\n");
    printf("Digite o valor do Offset (sera o valor de partida da rampa): ");
    scanf("%f", &offset_input);

    printf("Digite o valor do dt_step (tamanho de cada degrau de decremento): ");
    scanf("%f", &dt_step_input);

    printf("Digite o valor de n_step (a cada quantos passos o dado sera decrementado): ");
    scanf("%d", &n_step_input);

    if (n_step_input <= 0) {
        printf("Erro: n_step deve ser um valor inteiro positivo.\n");
        return 1; // Retorna com erro
    }
    if (offset_input < 0) {
        printf("Erro: Offset deve ser um valor positivo para uma rampa decrescente ate zero.\n");
        return 1;
    }
    if (dt_step_input <= 0) {
        printf("Erro: dt_step deve ser um valor positivo para uma rampa decrescente.\n");
        return 1;
    }

    gerarRampaInvertida(offset_input, dt_step_input, n_step_input);

    printf("\n");
    // Adiciona uma pausa para que o usuário possa ver a saída (específico para Windows)
    #ifdef _WIN32
        system("pause"); 
    #endif

    return 0;
}
