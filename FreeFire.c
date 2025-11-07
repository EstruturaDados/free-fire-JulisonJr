
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_Itens 10
#define MAX_Nome 50
#define MAX_Tipo 30

typedef struct {
    char nome[MAX_Nome];
    char tipo[MAX_Tipo];
    int quantidade;
    int prioridade; // 1 a 5 (5 = mais alta)
} Item;

typedef enum {
    Nome = 1,
    Tipo,
    Prioridade
} CriterioOrdenacao;

//Variáveis globais
Item mochila[MAX_Itens];
int numItens = 0;
int comparacoes = 0; // contador de comparações na ordenação
int ordenadaPorNome = 0;

// Utilitários
void limparEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) ;
}

void trim_newline(char *s) {
    size_t l = strlen(s);
    if (l > 0 && s[l-1] == '\n') s[l-1] = '\0';
}

void limparTela() {
    for (int i = 0; i < 40; i++) putchar('\n');
}

//exibição e menu
void exibirMenu() {
    printf("===== PLANO DE FUGA - MOCHILA (MÁX %d Itens) =====\n", MAX_Itens);
    printf("Itens atual: %d | Ordenada por nome: %s\n", numItens, ordenadaPorNome ? "SIM" : "NÃO");
    printf("1 - Adicionar um item\n");
    printf("2 - Remover um item\n");
    printf("3 - Listar todos os itens\n");
    printf("4 - Ordenar itens (nome, tipo, prioridade)\n");
    printf("5 - Busca binaria por nome\n");
    printf("6 - Salvar mochila em arquivo\n");
    printf("7 - Carregar mochila de arquivo\n");
    printf("0 - Sair\n");
    printf("Escolha: ");
}

//operações sobre a mochila
void inserirItem() {
    if (numItens >= MAX_Itens) {
        printf("Mochila cheia! Remova um item antes de inserir novamente.\n");
        return;
    }

    Item it;
    printf("Nome: ");
    if (!fgets(it.nome, MAX_Nome, stdin)) return;
    trim_newline(it.nome);

    printf("Tipo: ");
    if (!fgets(it.tipo, MAX_Tipo, stdin)) return;
    trim_newline(it.tipo);

    printf("Quantidade: ");
    if (scanf("%d", &it.quantidade) != 1) { limparEntrada(); printf("Entrada invalida.\n"); return; }
    limparEntrada();

    printf("Prioridade (1-5, 5 = mais alta): ");
    if (scanf("%d", &it.prioridade) != 1) { limparEntrada(); printf("Entrada invalida.\n"); return; }
    limparEntrada();

    if (it.prioridade < 1) it.prioridade = 1;
    if (it.prioridade > 5) it.prioridade = 5;

    mochila[numItens++] = it;
    ordenadaPorNome = 0; // qualquer inserção quebra ordenação por nome
    printf("Item '%s' inserido com sucesso.\n", it.nome);
}

void removerItem() {
    if (numItens == 0) { printf("Mochila vazia.\n"); return; }

    char nomeBusca[MAX_Nome];
    printf("Nome do item a remover: ");
    if (!fgets(nomeBusca, MAX_Nome, stdin)) return;
    trim_newline(nomeBusca);

    int pos = -1;
    for (int i = 0; i < numItens; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) { pos = i; break; }
    }

    if (pos == -1) {
        printf("Item '%s' não encontrado.\n", nomeBusca);
        return;
    }

    for (int i = pos; i < numItens - 1; i++) mochila[i] = mochila[i + 1];
    numItens--;
    printf("Item '%s' removido.\n", nomeBusca);
}

void listarItens() {
    if (numItens == 0) { printf("mochila vazia.\n"); return; }

    printf("\n%-3s | %-20s | %-12s | %8s | %-9s\n", "#", "NOME", "TIPO", "QUANT", "PRIORID.");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < numItens; i++) {
        printf("%3d | %-20s | %-12s | %8d | %-9d\n",
               i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
    }
    printf("\n");
}

//comparação paramétrica para ordenação
int compara(const Item *a, const Item *b, CriterioOrdenacao crit) {
    if (crit == Nome) {
        return strcmp(a->nome, b->nome);
    } else if (crit == Tipo) {
        int r = strcmp(a->tipo, b->tipo);
        if (r != 0) return r;
        return strcmp(a->nome, b->nome); // secundário por nome
    } else { // PRIORIDADE -> ordem decrescente (5 -> 1)
        if (a->prioridade == b->prioridade) return strcmp(a->nome, b->nome);
        return b->prioridade - a->prioridade;
    }
}

//insertion sort com contador de comparações
void insertionSort(CriterioOrdenacao crit) {
    comparacoes = 0;
    for (int i = 1; i < numItens; i++) {
        Item chave = mochila[i];
        int j = i - 1;
        while (j >= 0) {
            comparacoes++;
            if (compara(&mochila[j], &chave, crit) > 0) {
                mochila[j + 1] = mochila[j];
                j--;
            } else break;
        }
        mochila[j + 1] = chave;
    }
    if (crit == Nome) ordenadaPorNome = 1; else ordenadaPorNome = 0;
}

void menuDeOrdenacao() {
    printf("Escolha o criterio de ordenação:\n");
    printf("1 - Nome (alfabetica)\n");
    printf("2 - Tipo (alfabetica, secundario: nome)\n");
    printf("3 - Prioridade (decrescente)\n");
    printf("Escolha: ");
    int op;
    if (scanf("%d", &op) != 1) { limparEntrada(); printf("Entrada invalida.\n"); return; }
    limparEntrada();

    if (op < 1 || op > 3) { printf("Opção invalida.\n"); return; }
    insertionSort((CriterioOrdenacao)op);
    printf("Ordenação concluida. Comparações: %d\n", comparacoes);
}

//busca binária por nome (necessita que mochila seja ordenada por nome)
int buscaBinariaPorNome(const char *nomeBusca) {
    if (!ordenadaPorNome) {
        printf("A mochila não esta ordenada por nome. Ordene por nome antes de usar busca binaria.\n");
        return -1;
    }

    int inicio = 0, fim = numItens - 1;
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        int cmp = strcmp(nomeBusca, mochila[meio].nome);
        if (cmp == 0) return meio;
        else if (cmp < 0) fim = meio - 1;
        else inicio = meio + 1;
    }
    return -1;
}

//  salvar / carregar
void salvarArquivo(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "wb");
    if (!f) { printf("Erro ao abrir arquivo para escrita.\n"); return; }
    fwrite(&numItens, sizeof(int), 1, f);
    fwrite(mochila, sizeof(Item), numItens, f);
    fclose(f);
    printf("Mochila salva em '%s' (itens: %d).\n", nomeArquivo, numItens);
}

void carregarArquivo(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "rb");
    if (!f) { printf("Erro ao abrir arquivo para leitura.\n"); return; }
    int n;
    if (fread(&n, sizeof(int), 1, f) != 1) { printf("Arquivo invalido ou corrompido.\n"); fclose(f); return; }
    if (n < 0 || n > MAX_Itens) { printf("Arquivo possui número de itens invalido.\n"); fclose(f); return; }
    if (fread(mochila, sizeof(Item), n, f) != (size_t)n) { printf("Falha ao ler itens.\n"); fclose(f); return; }
    numItens = n;
    ordenadaPorNome = 0;
    fclose(f);
    printf("Mochila carregada de '%s' (itens: %d).\n", nomeArquivo, numItens);
}

//função principal
int main(void) {
    int rodando = 1;
    while (rodando) {
        exibirMenu();
        int opcao;
        if (scanf("%d", &opcao) != 1) { limparEntrada(); printf("Entrada invalida.\n"); continue; }
        limparEntrada();

        switch (opcao) {
            case 1: inserirItem(); break;
            case 2: removerItem(); break;
            case 3: listarItens(); break;
            case 4: menuDeOrdenacao(); break;
            case 5: {
                char nomeBusca[MAX_Nome];
                printf("Nome a buscar: ");
                if (!fgets(nomeBusca, MAX_Nome, stdin)) break;
                trim_newline(nomeBusca);
                int pos = buscaBinariaPorNome(nomeBusca);
                if (pos >= 0) {
                    Item *it = &mochila[pos];
                    printf("Item encontrado (posição %d): %s | %s | Qtd: %d | Pri: %d\n",
                           pos + 1, it->nome, it->tipo, it->quantidade, it->prioridade);
                } else printf("Item '%s' não encontrado.\n", nomeBusca);
                break;
            }
            case 6: {
                char arquivo[100];
                printf("Nome do arquivo para salvar: ");
                if (!fgets(arquivo, sizeof(arquivo), stdin)) break;
                trim_newline(arquivo);
                salvarArquivo(arquivo);
                break;
            }
            case 7: {
                char arquivo[100];
                printf("Nome do arquivo para carregar: ");
                if (!fgets(arquivo, sizeof(arquivo), stdin)) break;
                trim_newline(arquivo);
                carregarArquivo(arquivo);
                break;
            }
            case 0: rodando = 0; break;
            default: printf("Opção inválida. Tente novamente.\n");
        }

        if (rodando) {
            printf("\nPressione Enter para continuar...");
            getchar();
            limparTela();
        }
    }

    printf("Saindo... Até a próxima fuga da ilha!\n");
    return 0;
}