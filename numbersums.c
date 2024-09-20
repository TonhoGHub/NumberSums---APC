// Trabalho de conclusão de semestre - APC - 2024.1
// Aluno: Antonio Barros Coelho - 241038100

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #define CLEAR "cls"     // Comandos para limpar o terminal
#else
    #define CLEAR "clear"
#endif

typedef struct 
{
    char nickname[21];      // Informações que são armazenadas no ranking
    int pontos;
} Player;

// Variáveis globais
int n = 4, vidas = 5, fase = 0, dificuldade = 1;
int matriz_jogo[8][8], espelho[8][8];
int somacol[15], somalin[15];
int action, configs, count;
char modo[15] = "INICIANTE";
Player player;
FILE *ranking_file;

// Protótipos das funções auxiliares
int compare(const void *a, const void *b);
void wait_enter(void);
void print_welcome(void);
void print_options(void);
void carrega_dados(void);
void print_jogo(void);
void print_settings(void);
void zera_ranking(void);
void print_dificuldade(void);
void print_tutorial(void);
void write_ranking(void);
void print_ranking(void);

int main(void)
{
    system(CLEAR);

    print_welcome();

    system(CLEAR);

    action = 0;
    player.pontos = 0;

    while (action != 5)
    {
        system(CLEAR);

        print_options();

        switch (action)
        {
            case 1: // Jogar
                system(CLEAR);
                carrega_dados();
                print_jogo();
                break;
            case 2: // Settings
                system(CLEAR);
                print_settings();
                break;
            case 3: // Tutorial
                system(CLEAR);
                print_tutorial();
                break;
            case 4: // Ranking
                system(CLEAR);
                print_ranking();           
                break;
            case 5: // Sair
                system(CLEAR);
                printf("Obrigado por jogar o JOGO DAS SOMAS, %s. Volte sempre!", player.nickname);
                getchar();
                getchar();
                break;
            default:
                printf("Falha ao acessar. Tente novamente.\n");
                getchar();
                break;
        }
    }
    
    system(CLEAR);
    return 0;
}

int compare(const void *a, const void *b)
{
    Player *playerA = (Player *)a;
    Player *playerB = (Player *)b;

    int dif_pontos = playerB->pontos - playerA->pontos;

    if (dif_pontos != 0) return dif_pontos;

    return strcmp(playerA->nickname, playerB->nickname);
}

void wait_enter(void)
{
    printf("Pressione a tecla <enter> para prosseguir...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

void print_welcome(void)
{
    printf("\nSeja bem vindo(a) ao Jogo das Somas - APC!\n");
    printf("\nInforme o seu nickname: ");
    scanf("%s", player.nickname);
}

void print_options(void)
{
    printf("*** JOGO DAS SOMAS ***\n");
    printf("\n1 - Jogar\n");
    printf("2 - Settings\n");
    printf("3 - Tutorial\n");
    printf("4 - Ranking\n");
    printf("5 - Sair\n");

    printf("\nDigite o que quer fazer: ");
    scanf("%d", &action);
}

void carrega_dados(void)
{
    FILE *fp = NULL;
    char num;

    if (dificuldade == 1)
    {
        n = 4;
        fp = fopen("iniciante.txt", "r");
        count = (60 * fase);        // Cada fase nesse nível possui 60 caracteres
    }
    else if (dificuldade == 2)
    {
        n = 6;
        fp = fopen("intermediario.txt", "r");
        count = (112 * fase);       // Cada fase nesse nível possui 112 caracteres
    }
    else if (dificuldade == 3)
    {
        n = 7;
        fp = fopen("avancado.txt", "r");
        count = (144 * fase);       // Cada fase nesse nível possui 144 caracteres
    }

    if (fp == NULL)
    {
        perror("Falha ao abrir o arquivo.");
        return;
    }

    fseek(fp, count, SEEK_SET);
    
    for (int i = 0; i < n; i++)             // Loop de preenchimento da matriz principal
    {
        for (int j = 0; j < n; j++)
        {
            do
            {
                fscanf(fp, "%c", &num);
                if (num == '\n') count++;
            } while (num == '\n');
            
            matriz_jogo[i][j] = num - '0';
            count++;
        }
    }

    fseek(fp, count, SEEK_SET);

    for (int i = 0; i < (2 * n); i++)       // Loop de preenchimento do vetor que guarda as somas das colunas
    {
        do 
        {
            fscanf(fp, "%c", &num);
            if (num == '\n') count++;    
        } while (num == '\n' || num == ' ');

        somacol[i] = num - '0';
        count++;
    }

    fseek(fp, count, SEEK_SET);

    for (int i = 0; i < (2 * n); i++)       // Loop de preenchimento do vetor que guarda as somas das linhas
    {
        do 
        {
            fscanf(fp, "%c", &num);
            if (num == '\n') count++;
        } while (num == '\n');

        somalin[i] = num - '0';
        count++;
    }

    fseek(fp, count, SEEK_SET);

    for (int i = 0; i < n; i++)         // Loop de preenchimento da matriz espelho
    {
        for (int j = 0; j < n; j++)
        {
            do
            {
                fscanf(fp, "%c", &num);
                if (num == '\n' || num == '*') count++;
            } while (num == '\n' || num == '*');

            espelho[i][j] = num - '0';
            count++;
        }
    }

    count += 2;     // Conta o asterisco e o último "\n"
    fseek(fp, count, SEEK_SET);

    fclose(fp);
}

void print_jogo(void)
{
    int linha, coluna;
 
    while (vidas >= 0 && fase <= 4)
    {
        int zerou_coluna = 1, zerou_linha = 1;
        
        if (vidas == 0)
        {
            printf("\nVoce gastou todas as suas vidas. Tente novamente.\n");
            dificuldade = 1;
            fase = 0;
            vidas = 5;
            player.pontos = 0;
            wait_enter();
            break;
        }
        if (fase == 4)
        {
            if (dificuldade == 3)
            {
                system(CLEAR);
                printf("\nParabens, %s! Voce ZEROU O JOGO!\n", player.nickname);
                wait_enter();
                vidas = 5;
                fase = 0;
                dificuldade = 1;
                player.pontos = 0;
                break;
            }
            else
            {
                system(CLEAR);
                printf("\nParabens, %s! Voce ganhou a dificuldade %s!\n", player.nickname, modo);
                wait_enter();
                vidas = 5;
                fase = 0;
                dificuldade++;
                break;
            }
        }
        
        system(CLEAR);

        int somacol_local = 1;
        int somalin_local = 1;

        for (int i = 0; i < (2 * n); i += 2)
        {
            if (somacol[i] != 666) 
            {
                zerou_coluna = 0;
            }
        }
        for (int i = 0; i < n; i++)
        {
            if (somalin[i*2] != 666)
            {
                zerou_linha = 0;
            }
        }

        if (zerou_coluna == 1 && zerou_linha == 1) 
        {
            printf("\nVoce ganhou a fase %d! Muito bem, %s!\n", (fase + 1), player.nickname);
            wait_enter();
            vidas = 5;
            fase++;

            if (dificuldade == 1) player.pontos += 50;
            else if (dificuldade == 2) player.pontos += 100;
            else if (dificuldade ==  3) player.pontos += 200;

            write_ranking();
            carrega_dados();
            continue;
        }

        if (dificuldade == 1) strcpy(modo, "INICIANTE");
        else if (dificuldade == 2) strcpy(modo, "INTERMEDIARIO");
        else if (dificuldade == 3) strcpy(modo, "AVANCADO");

        printf("Dificuldade: %s | Fase: %d\n", modo, (fase + 1));
        printf("Pontuacao do jogador: %d\n", player.pontos);
        printf("\n    ");
        for (int i = 0; i < (2 * n); i += 2)
        {
            if (somacol[i] == 666) 
            {
                printf("   ");
            }
            else 
            {
                printf("%d%d ", somacol[i], somacol[i+1]);
            }
        }
        printf("\n   -");
        for (int i = 0; i < n; i++)
        {
            printf("---");
        }
        printf("\n");
        for (int i = 0; i < n; i++)
        {
            if (somalin[i*2] == 666)
            {
                printf("   | ");
            }
            else 
            {
                printf("%d%d | ", somalin[i*2], somalin[i*2+1]);
            }
            
            for (int j = 0; j < n; j++)
            {
                if (matriz_jogo[i][j] == 666)
                {
                    printf("   ");
                }
                else
                {
                    printf("%d  ", matriz_jogo[i][j]);
                }               
            }
            printf("\n");
        }

        printf("\n*** Voce tem %d vidas ***\n", vidas);
        printf("\nDigite a linha e a coluna do elemento a ser apagado: ");
        scanf("%d %d", &linha, &coluna);

        if (espelho[linha-1][coluna-1] == 0)
        {
            printf("\nMuito bem! ");
            wait_enter();

            matriz_jogo[linha-1][coluna-1] = 666;
            espelho[linha-1][coluna-1] = 666;

            for (int i = 0; i < n; i++)
            {
                somacol_local *= espelho[i][coluna-1];
                somalin_local *= espelho[linha-1][i];
            }
            if (somacol_local != 0)
            {
                printf("\nParabens! Voce fechou a coluna %d. ", coluna);
                somacol[(coluna-1)*2] = 666;
                wait_enter();
            }
            if (somalin_local != 0)
            {
                printf("\nParabens! Voce fechou a linha %d. ", linha);
                somalin[(linha-1)*2] = 666;
                wait_enter();
            } 
        }
        else if (espelho[linha-1][coluna-1] == 1)
        {
            printf("\nNao foi dessa vez... ");
            wait_enter();

            vidas--;
        }
        else if (espelho[linha-1][coluna-1] == 666)
        {
            printf("\nVoce ja apagou este elemento. ");
            wait_enter();
        }
    } 
}

void print_settings(void)
{
    configs = 0;
    while (configs != 3)
    {
        system(CLEAR);
        char confirm[2];

        printf("*** SETTINGS - JOGO DAS SOMAS ***\n");
        printf("\n1 - Zerar Ranking\n");
        printf("2 - Modo de dificuldade\n");
        printf("3 - Voltar ao menu principal\n");
        printf("\nDigite o que quer fazer: ");
        scanf("%d", &configs);

        switch (configs)
        {
            case 1: // Zerar ranking
                printf("Confirma reinicializar o ranking? (S/N) ");
                scanf("%s", confirm);
                if (strcmp(confirm, "S") == 0)
                {
                    zera_ranking();
                    printf("\nRanking zerado.\n");
                    printf("\n");
                    wait_enter();
                } 
                break;
            case 2: // Dificuldade
                print_dificuldade();
                break;
            case 3: // Menu principal
                break;
            default: 
                printf("Opcao invalida. Tente novamente.");
                break;
        }
    }  
}

void zera_ranking(void)
{
    ranking_file = fopen("ranking.bin", "wb");    // Quando um arquivo é aberto para escrita, seu conteúdo antigo é automaticamente apagado.
    if (ranking_file == NULL)
    {
        perror("Erro ao abrir o arquivo.");
        return;
    }

    player.pontos = 0;

    fclose(ranking_file);
}

void print_dificuldade(void)
{
    system(CLEAR);

    printf("*** ESCOLHA O MODO DE JOGO ***\n");
    printf("\n1 - Iniciante\n");
    printf("2 - Intermediario\n");
    printf("3 - Avancado\n");
    printf("4 - Retornar\n");

    printf("\nDigite o que quer fazer: ");
    scanf("%d", &dificuldade);

    switch (dificuldade)
    {
        case 1: // Iniciante
            strcpy(modo, "INICIANTE");
            // n = 4;
            dificuldade = 1;
            break;
        case 2: // Intermediário
            strcpy(modo, "INTERMEDIARIO");
            // n = 6;
            dificuldade = 2;
            break;
        case 3: // Avançado
            strcpy(modo, "AVANCADO");
            // n = 7;
            dificuldade = 3;
            break;
        case 4: // Retornar
            return;
        default:
            printf("Opcao invalida. Tente novamente.\n");
            return;
    }

    vidas = 5;

    printf("\nConfigurado para o modo %s.\n", modo);
    printf("\n");
    wait_enter();
}

void print_tutorial(void)
{
    printf("*** TUTORIAL - JOGO DAS SOMAS ***\n");
    printf("\nInstrucoes de jogo:\n");
    printf("O Jogo das Somas e um jogo de raciocinio em que o jogador deve \neliminar numeros de um grid 2D para que as somas restantes de cada \nlinha e coluna correspondam aos valores-alvo indicados.\n");
    printf("\nObjetivo do jogo:\n");
    printf("Eliminar numeros da matriz de forma que as somas dos numeros \nrestantes em cada linha e coluna sejam iguais aos valores \nindicados no topo das colunas e na frente das linhas.\n");
    printf("\nComo jogar:\n");
    printf("1. O jogo comeca com uma matriz de numeros de 1 a 9. \nCada linha e coluna tem um somatorio desejado.\n");
    printf("2. Voce deve escolher um numero para eliminar indicando \nsua posicao (linha e coluna).\n");
    printf("3. Se a eliminacao for correta (nao exceder a soma desejada), \no numero desaparece da matriz. Caso contrario, voce perde uma vida.\n");
    printf("4. Voce tem um total de 5 vidas para completar o nivel.\n");
    printf("5. Quando todas as linhas e colunas atingirem suas somas desejadas, \no nivel e concluido e voce avanca para o proximo.\n");
    printf("\nModos de jogo:\n");
    printf("Existem tres modos de dificuldade: \nIniciante (4x4), Intermediario (6x6), e Avancado (7x7).\n");
    printf("Cada modo tem seu proprio conjunto de matrizes e pontuacoes: \n50 pontos para Iniciante, 100 para Intermediario e 200 para Avancado.\n");
    printf("\nPontuacao e Ranking:\n");
    printf("A pontuacao e acumulada a cada nivel completado.\n");
    printf("O ranking e salvo em um arquivo e pode ser \nconsultado ou reiniciado nas configuracoes.\n");
    printf("\nFim do jogo:\n");
    printf("O jogo termina quando voce perde todas as vidas ou quando \ncompleta todos os niveis do modo Avancado.\n");
    printf("\nBoa sorte! Divirta-se!\n");
    printf("\n");

    wait_enter();
}

void write_ranking(void)
{
    ranking_file = fopen("ranking.bin", "a");
    if (ranking_file == NULL)
    {
        perror("Erro ao abrir o arquivo.");
        return;
    }

    fprintf(ranking_file, "%20s %d\n", player.nickname, player.pontos);

    fclose(ranking_file);
}

void print_ranking(void)
{
    Player ranking[200];
    char duplicatas[200][21];
    int total_players = 0, total_duplicatas = 0;

    ranking_file = fopen("ranking.bin", "r");
    if (ranking_file == NULL)
    {
        perror("Erro ao abrir o arquivo.");
        return;
    }

    while (fscanf(ranking_file, "%s %d", ranking[total_players].nickname, &ranking[total_players].pontos) == 2)
    {
        total_players++;
    }

    fclose(ranking_file);

    qsort(ranking, total_players, sizeof(Player), compare);

    printf("*** RANKING - JOGO DAS SOMAS ***\n");
    printf("--------------------------------\n");
    printf("\n");
    for (int i = 0; i < total_players; i++)
    {
        int impresso = 0;
        for (int j = 0; j < total_duplicatas; j++)
        {
            if (strcmp(ranking[i].nickname, duplicatas[j]) == 0)
            {
                impresso = 1;
                break;
            }
        }

        if (!impresso)
        {
            printf("%20s - %d pontos\n", ranking[i].nickname, ranking[i].pontos);

            strcpy(duplicatas[total_duplicatas], ranking[i].nickname);
            total_duplicatas++;
        }
    }
    printf("\n--------------------------------\n");
    wait_enter();
}