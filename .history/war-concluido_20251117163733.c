#include <stdio.h>
#include <stdlib.h> // Para malloc, free, rand, srand, qsort
#include <string.h> // Para strcpy, strcmp
#include <time.h>   // Para srand

/*
================================================================================
PARTE 1: CADASTRO COM VETOR DE STRUCTS
================================================================================
Objetivo: Estruturar os dados básicos (nome, cor, tropas) em um vetor de structs.
*/

// Define a estrutura para um Território
typedef struct {
    char nome[50];
    char cor_dono[20]; // "cor" do jogador que domina o território
    int tropas;
} Territorio;

// Constante para o número de territórios (Apenas para este exemplo)
// Um jogo real de War (Risk) tem 42 territórios.
#define NUM_TERRITORIOS_EXEMPLO 2 


/*
================================================================================
PARTE 2: FUNCIONALIDADE DE ATAQUE (PONTEIROS E ALOCAÇÃO DINÂMICA)
================================================================================
Objetivo: Simular uma batalha entre dois territórios.
- Usa PONTEIROS para modificar diretamente os territórios (atacante, defensor).
- Usa ALOCAÇÃO DINÂMICA para os arrays dos dados, como solicitado.
*/

// Função auxiliar para ordenar os dados em ordem decrescente
int comparar_int_desc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

// Função que simula um ataque
void simular_ataque(Territorio* atacante, Territorio* defensor) {
    printf("\n--- SIMULANDO ATAQUE ---\n");
    printf("%s (tropas: %d) ataca %s (tropas: %d)\n",
           atacante->nome, atacante->tropas,
           defensor->nome, defensor->tropas);

    // 1. Verificar validade do ataque
    if (atacante->tropas <= 1) {
        printf("Ataque invalido! O atacante deve ter mais de 1 tropa.\n");
        return;
    }
    // Não podem atacar a si mesmos ou ao mesmo dono
    if (strcmp(atacante->cor_dono, defensor->cor_dono) == 0) {
        printf("Ataque invalido! Nao pode atacar um territorio proprio.\n");
        return;
    }

    // 2. Definir número de dados (Regra do War)
    int dados_atk_num = (atacante->tropas > 3) ? 3 : (atacante->tropas - 1);
    int dados_def_num = (defensor->tropas > 1) ? 2 : 1;

    // 3. ALOCAÇÃO DINÂMICA para os dados (conforme solicitado)
    int* dados_ataque = (int*) malloc(dados_atk_num * sizeof(int));
    int* dados_defesa = (int*) malloc(dados_def_num * sizeof(int));

    // Verificar falha na alocação
    if (dados_ataque == NULL || dados_defesa == NULL) {
        printf("Erro fatal: Falha na alocacao de memoria para os dados!\n");
        free(dados_ataque); // free(NULL) é seguro
        free(dados_defesa);
        return;
    }

    // 4. Rolar os dados
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

    // 5. Ordenar os dados (maior para o menor)
    qsort(dados_ataque, dados_atk_num, sizeof(int), comparar_int_desc);
    qsort(dados_defesa, dados_def_num, sizeof(int), comparar_int_desc);

    // 6. Comparar dados e atualizar tropas (usando PONTEIROS)
    int comparacoes = (dados_atk_num < dados_def_num) ? dados_atk_num : dados_def_num;
    int perdas_atk = 0;
    int perdas_def = 0;

    for (int i = 0; i < comparacoes; i++) {
        if (dados_ataque[i] > dados_defesa[i]) {
            perdas_def++; // Defesa perde
        } else {
            perdas_atk++; // Ataque perde (defesa ganha no empate)
        }
    }

    // Aplica perdas
    atacante->tropas -= perdas_atk;
    defensor->tropas -= perdas_def;

    printf("Resultado: Ataque perdeu %d tropas. Defesa perdeu %d tropas.\n", perdas_atk, perdas_def);

    // 7. Verificar conquista
    if (defensor->tropas <= 0) {
        printf("\nCONQUISTA! %s (%s) dominou %s.\n",
               atacante->nome, atacante->cor_dono, defensor->nome);
        
        // Atualiza o dono (cor) do território
        strcpy(defensor->cor_dono, atacante->cor_dono);
        
        // Move as tropas (regra: move o mesmo número de dados que usou)
        int tropas_movidas = dados_atk_num;
        defensor->tropas = tropas_movidas;
        atacante->tropas -= tropas_movidas;

        printf("%s moveu %d tropas para o novo territorio.\n", atacante->nome, tropas_movidas);
    }

    // 8. LIBERAR A MEMÓRIA ALOCADA
    free(dados_ataque);
    free(dados_defesa);
}


/*
================================================================================
PARTE 3: SISTEMA DE MISSÕES (PONTEIROS E ALOCAÇÃO DINÂMICA)
================================================================================
Objetivo: Verificar uma condição de vitória (missão).
- Usa ALOCAÇÃO DINÂMICA para criar a missão de um jogador.
- Usa PONTEIROS para verificar o estado do jogo (ponteiro para a missão,
  ponteiro para o vetor de territórios).
*/

// Tipos de missões (simplificado)
typedef enum {
    DESTRUIR_JOGADOR,
    OCUPAR_TERRITORIOS
} TipoMissao;

// Estrutura da Missão
typedef struct {
    TipoMissao tipo;
    char descricao[150];
    
    // Alvos da missão
    char alvo_cor_jogador[20]; // Cor do jogador a ser destruído
    int alvo_num_territorios;  // Número de territórios a ocupar

} Missao;


// Função que cria e atribui uma missão (com ALOCAÇÃO DINÂMICA)
Missao* atribuir_missao_jogador(const char* cor_alvo) {
    // 1. ALOCAÇÃO DINÂMICA para a struct da missão
    Missao* missao_jogador = (Missao*) malloc(sizeof(Missao));
    if (missao_jogador == NULL) {
        printf("Erro fatal: Falha ao alocar memoria para a missao!\n");
        return NULL;
    }

    // 2. Configura a missão (exemplo: destruir jogador "Azul")
    missao_jogador->tipo = DESTRUIR_JOGADOR;
    strcpy(missao_jogador->alvo_cor_jogador, cor_alvo);
    missao_jogador->alvo_num_territorios = 0; // Não usado neste tipo

    sprintf(missao_jogador->descricao, "Objetivo: Destruir o jogador %s.", cor_alvo);
    
    // Retorna o PONTEIRO para a missão alocada
    return missao_jogador;
}

// Verifica se a missão foi concluída (usando PONTEIROS)
int verificar_missao(Missao* missao, Territorio* todos_territorios, int num_total_territorios) {
    if (missao == NULL) return 0; // Missão não existe

    if (missao->tipo == DESTRUIR_JOGADOR) {
        // Verifica se o jogador "alvo" ainda possui territórios
        int territorios_alvo = 0;
        for (int i = 0; i < num_total_territorios; i++) {
            // Compara a cor do dono do território com a cor alvo da missão
            if (strcmp(todos_territorios[i].cor_dono, missao->alvo_cor_jogador) == 0) {
                territorios_alvo++;
            }
        }
        
        if (territorios_alvo == 0) {
            return 1; // Vitória! O jogador alvo foi destruído.
        }
    }
    // (Aqui iriam as verificações para outros tipos de missão, ex: OCUPAR_TERRITORIOS)

    return 0; // Missão ainda não concluída
}


/*
================================================================================
FUNÇÃO PRINCIPAL (MAIN) - Orquestrando o Jogo
================================================================================
*/
int main() {
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    printf("========= INICIALIZANDO JOGO WAR (SIMPLIFICADO) =========\n");

    // --- Parte 1: Criando o vetor de structs de Territórios ---
    Territorio territorios_jogo[NUM_TERRITORIOS_EXEMPLO];

    // Cadastrando dados de exemplo
    strcpy(territorios_jogo[0].nome, "Brasil");
    strcpy(territorios_jogo[0].cor_dono, "Verde"); // Jogador Verde
    territorios_jogo[0].tropas = 5;

    strcpy(territorios_jogo[1].nome, "Argentina");
    strcpy(territorios_jogo[1].cor_dono, "Azul"); // Jogador Azul
    territorios_jogo[1].tropas = 3;

    printf("Estado Inicial do Jogo:\n");
    printf("Territorio 1: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[0].nome, territorios_jogo[0].cor_dono, territorios_jogo[0].tropas);
    printf("Territorio 2: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[1].nome, territorios_jogo[1].cor_dono, territorios_jogo[1].tropas);

    
    // --- Parte 3: Atribuindo missão (com alocação dinâmica) ---
    // Jogador "Verde" recebe a missão de destruir o "Azul"
    Missao* missao_jogador_verde = atribuir_missao_jogador("Azul");
    if(missao_jogador_verde != NULL) {
        printf("\nMissao do Jogador Verde: %s\n", missao_jogador_verde->descricao);
    }

    // --- Parte 2: Simulando o ataque (com ponteiros e alocação dinâmica) ---
    // O jogador Verde (Brasil) ataca o Azul (Argentina)
    // Passamos os endereços (ponteiros) dos territórios
    simular_ataque(&territorios_jogo[0], &territorios_jogo[1]);

    
    // --- Verificando o estado final ---
    printf("\n========= ESTADO FINAL DO JOGO =========\n");
    printf("Territorio 1: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[0].nome, territorios_jogo[0].cor_dono, territorios_jogo[0].tropas);
    printf("Territorio 2: %s (Dono: %s, Tropas: %d)\n",
           territorios_jogo[1].nome, territorios_jogo[1].cor_dono, territorios_jogo[1].tropas);

    // --- Parte 3: Verificando a missão ---
    printf("\nVerificando missao do Jogador Verde...\n");
    
    // Passamos o ponteiro da missão e o ponteiro do vetor de territórios
    if (verificar_missao(missao_jogador_verde, territorios_jogo, NUM_TERRITORIOS_EXEMPLO)) {
        printf("VITORIA! O Jogador Verde completou sua missao!\n");
    } else {
        printf("Missao ainda nao concluida. O jogador %s ainda possui territorios.\n",
               missao_jogador_verde->alvo_cor_jogador);
    }
    
    // --- Limpeza da Memória ---
    // Libera a memória alocada dinamicamente para a missão
    free(missao_jogador_verde);
    missao_jogador_verde = NULL;
    
    printf("\n========= FIM DA SIMULACAO =========\n");

    return 0;
}