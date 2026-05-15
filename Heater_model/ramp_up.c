#include <stdio.h>
#include <stdlib.h> // Para a função system("pause") no Windows

void gerarRampa(float offset, float dt_step, int n_step) {
    // Calcula o número total de degraus para atingir o offset
    // Se dt_step for 0, evitar divisão por zero
    if (dt_step == 0) {
        printf("Erro: dt_step nao pode ser zero.\n");
        return;
    }
    
    int num_degraus_totais = (int)(offset / dt_step);
    
    printf("Gerando rampa com:\n");
    printf("  Offset: %.2f\n", offset);
    printf("  dt_step: %.2f\n", dt_step);
    printf("  n_step: %d\n", n_step);
    printf("  Numero total de degraus para atingir o offset: %d\n", num_degraus_totais);
    printf("\n");

    float valor_atual = 0.0;
    int contador_passos = 0;
    int degraus_gerados = 0;

    printf("Passo\tValor da Rampa\n");
    printf("-----\t--------------\n");

    // Loop para gerar a rampa até atingir ou ultrapassar o offset
    while (degraus_gerados <= num_degraus_totais) {
        // Incrementa o valor da rampa a cada n_step passos
        if (contador_passos % n_step == 0 && contador_passos != 0) {
            valor_atual += dt_step;
            degraus_gerados++;
        }
        
        // Garante que o último valor não ultrapasse o offset se for o caso
        if (valor_atual > offset && degraus_gerados > num_degraus_totais) {
            valor_atual = offset;
        }

        printf("%d\t%.2f\n", contador_passos, valor_atual);
        contador_passos++;

        // Condição de saída adicional para evitar loop infinito em casos específicos
        if (valor_atual >= offset && degraus_gerados >= num_degraus_totais) {
            break; 
        }
    }
    
    // Imprime o valor final se ele não foi impresso no último passo do loop
    if (valor_atual < offset) {
        valor_atual = offset;
        printf("%d\t%.2f\n", contador_passos, valor_atual);
    }
}

int main() {
    float offset_input;
    float dt_step_input;
    int n_step_input;

    printf("Digite o valor do Offset: ");
    scanf("%f", &offset_input);

    printf("Digite o valor do dt_step (tamanho de cada degrau): ");
    scanf("%f", &dt_step_input);

    printf("Digite o valor de n_step (a cada quantos passos o dado sera incrementado): ");
    scanf("%d", &n_step_input);

    if (n_step_input <= 0) {
        printf("Erro: n_step deve ser um valor inteiro positivo.\n");
        return 1; // Retorna com erro
    }

    gerarRampa(offset_input, dt_step_input, n_step_input);

    printf("\n");
    // Adiciona uma pausa para que o usuário possa ver a saída (específico para Windows)
    #ifdef _WIN32
        system("pause"); 
    #endif

    return 0;
}
