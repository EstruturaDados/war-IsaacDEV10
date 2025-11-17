#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nome[50];
    char cor_dono[20];
    int tropas;
} Territorio;

#define NUM_TERRITORIOS_EXEMPLO 2 

int comparar_int_desc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

void simular_ataque(Territorio* atacante, Territorio* defensor) {
    printf("\n--- SIMULANDO ATAQUE ---\n");
    printf("%s (tropas: %d) ataca %s (tropas: %d)\n",
           atacante->nome, atacante->tropas,
           defensor->nome, defensor->tropas);

    if (atacante->tropas <= 1) {
        printf("Ataque invalido! O atacante deve ter mais de 1 tropa.\n");
        return;
    }
    
    if (strcmp(atacante->cor_dono, defensor->cor_dono) == 0) {
        printf("Ataque invalido! Nao pode atacar um territorio proprio.\n");
        return;
    }

    int dados_atk_num = (atacante->tropas > 3) ? 3 : (atacante->tropas - 1);
    int dados_def_num = (defensor->tropas > 1) ? 2 : 1;

    int* dados_ataque = (int*) malloc(dados_atk_num * sizeof(int));
    int* dados_defesa = (int*) malloc(dados_def_num * sizeof(int));

    if (dados_ataque == NULL || dados_defesa == NULL) {
        printf("Erro fatal: Falha na alocacao de memoria para os dados!\n");
        free(dados_ataque);
        free(dados_defesa);
        return;
    }

    printf("Dados de Ataque (%d): ", dados_atk_num);
    for (int i = 0; i < dados_atk_num; i++) {
        dados_ataque[i] = rand() % 6 + 1;
        printf("%d ", dados_ataque[i]);
    }
    printf("\nDados de Defesa (%d): ", dados_def_num);
    for (int i = 0; i < dados_def_num; i++) {
        dados_defesa[i] = rand() % 6 + 1;
        printf("%d ", dados_defesa[i]);
    }
    printf("\n");

    qsort(dados_ataque, dados_atk_num, sizeof(int), comparar_int_desc);
    qsort(dados_defesa, dados_def_num, sizeof(int), comparar_int_desc);

    int comparacoes = (dados_atk_num < dados_def_num) ? dados_atk_num : dados_def_num;
    int perdas_atk = 0;
    int perdas_def = 0;

    for (int i = 0; i < comparacoes; i++) {
        if (dados_ataque[i] > dados_defesa[i]) {
            perdas_def++;
        } else {
            perdas_atk++;
        }
    }

    atacante->tropas -= perdas_atk;
    defensor->tropas -= perdas_def;

    printf("Resultado: Ataque perdeu %d tropas. Defesa perdeu %d tropas.\n", perdas_atk, perdas_def);

    if (defensor->tropas <= 0) {
        printf("\nCONQUISTA! %s (%s) dominou %s.\n",
               atacante->nome, atacante->cor_dono, defensor->nome);
        
        strcpy(defensor->cor_dono, atacante->cor_dono);
        
        int tropas_movidas = dados_atk_num;
        defensor->tropas = tropas_movidas;
        atacante->tropas -= tropas_movidas;

        printf("%s moveu %d tropas para o novo territorio.\n", atacante->nome, tropas_movidas);
    }

    free(dados_ataque);
    free(dados_defesa);
}

typedef enum {
    DESTRUIR_JOGADOR,
    OCUPAR_TERRITORIOS
} TipoMissao;

typedef struct {
    TipoMissao tipo;
    char descricao[150];
    
    char alvo_cor_jogador[20];
    int alvo_num_territorios;

} Missao;


Missao* atribuir_missao_jogador(const char* cor_alvo) {
    Missao* missao_jogador = (Missao*) malloc(sizeof(Missao));
    if (missao_jogador == NULL) {
        printf("Erro fatal: Falha ao alocar memoria para a missao!\n");
        return NULL;
    }

    missao_jogador->tipo = DESTRUIR_JOGADOR;
    strcpy(missao_jogador->alvo_cor_jogador, cor_alvo);
    missao_jogador->alvo_num_territorios = 0;

    sprintf(missao_jogador->descricao, "Objetivo: Destruir o jogador %s.", cor_alvo);
    
    return missao_jogador;
}

int verificar_missao(Missao* missao, Territorio* todos_territorios, int num_total_territorios) {
    if (missao == NULL) return 0;

    if (missao->tipo == DESTRUIR_JOGADOR) {
        int territorios_alvo = 0;
        for (int i = 0; i < num_total_territorios; i++) {
            if (strcmp(todos_territorios[i].cor_dono, missao->alvo_cor_jogador) == 0) {
                territorios_alvo++;
            }
        }
        
        if (territorios_alvo == 0) {
            return 1;
        }
    }

    return 0;
}


int main() {
    srand(time(NULL));

    printf("========= INICIALIZANDO JOGO WAR (SIMPLIFICADO) =========\n");

    Territorio territorios_jogo[NUM_TERRITORIOS_EXEMPLO];

    strcpy(territorios_jogo[0].nome, "Brasil");
    strcpy(territorios_jogo[0].cor_dono, "Verde");
    territorios_jogo[0].tropas = 5;

    strcpy(territorios_jogo[1].nome, "Argentina");
    strcpy(territorios_jogo[1].cor_dono, "Azul");
    territorios_jogo[1].tropas = 3;

    printf("Estado Inicial do Jogo:\n");
    printf("Territorio 1: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[0].nome, territorios_jogo[0].cor_dono, territorios_jogo[0].tropas);
    printf("Territorio 2: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[1].nome, territorios_jogo[1].cor_dono, territorios_jogo[1].tropas);

    
    Missao* missao_jogador_verde = atribuir_missao_jogador("Azul");
    if(missao_jogador_verde != NULL) {
        printf("\nMissao do Jogador Verde: %s\n", missao_jogador_verde->descricao);
    }

    simular_ataque(&territorios_jogo[0], &territorios_jogo[1]);

    
    printf("\n========= ESTADO FINAL DO JOGO =========\n");
    printf("Territorio 1: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[0].nome, territorios_jogo[0].cor_dono, territorios_jogo[0].tropas);
    printf("Territorio 2: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[1].nome, territorios_jogo[1].cor_dono, territorios_jogo[1].tropas);

    printf("\nVerificando missao do Jogador Verde...\n");
    
    if (verificar_missao(missao_jogador_verde, territorios_jogo, NUM_TERRITORIOS_EXEMPLO)) {
        printf("VITORIA! O Jogador Verde completou sua missao!\n");
    } else {
        printf("Missao ainda nao concluida. O jogador %s ainda possui territorios.\n",
               missao_jogador_verde->alvo_cor_jogador);
    }
    
    free(missao_jogador_verde);
    missao_jogador_verde = NULL;
    
    printf("\n========= FIM DA SIMULACAO =========\n");

    return 0;
}