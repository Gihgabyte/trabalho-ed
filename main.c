// Giovanna Carla Andrade da Silva - 202419060188
// Laura Santa Cruz -
// Ivan Francisco - 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100

typedef struct NoB {
    int n;                 // quantidade atual de chaves
    int folha;             // 1 = folha, 0 = interno

    char **chaves;         // vetor de strings

    struct NoB **filhos;   // vetor de ponteiros para filhos
} NoB;

char *copiarString(const char *s)
{
    char *nova = malloc(strlen(s)+1);

    strcpy(nova,s);

    return nova;
}

//variaveis globais
int d;
NoB *raiz = NULL;

NoB *criarNo(int folha)
{
    NoB *novo = malloc(sizeof(NoB));

    novo->folha = folha;
    novo->n = 0;

    novo->chaves = malloc((2*d + 1) * sizeof(char*));

    for(int i=0;i<2*d+1;i++){
        novo->chaves[i] = NULL;
    }

    novo->filhos = malloc((2*d + 2) * sizeof(NoB*));

    for(int i=0;i<2*d+2;i++)
        novo->filhos[i] = NULL;

    return novo;
}


NoB *buscar(NoB *p, char *chave, int *pos)
{
    if(p == NULL)
        return NULL;

    int i = 0;

    while(i < p->n &&
          strcmp(chave, p->chaves[i]) > 0)
        i++;

    if(i < p->n &&
       strcmp(chave, p->chaves[i]) == 0)
    {
        *pos = i;
        return p;
    }

    if(p->folha)
        return NULL;

    return buscar(p->filhos[i], chave, pos);
}


//inserção simples em página
void inserirOrdenado(NoB *p, char *chave)
{
    int i = p->n - 1;

    while(i >= 0 &&
          strcmp(chave, p->chaves[i]) < 0)
    {
        p->chaves[i+1] = p->chaves[i];
        i--;
    }

    p->chaves[i+1] = copiarString(chave);

    p->n++;
}



// split

void splitFilho(NoB *pai, int indice)
{
    NoB *y = pai->filhos[indice];
    NoB *z = criarNo(y->folha);

    int meio = d;

    z->n = d - 1;

    for(int j = 0; j < d - 1; j++)
    {
        z->chaves[j] = y->chaves[j + meio + 1];
        y->chaves[j + meio + 1] = NULL;
    }

    if(!y->folha)
    {
        for(int j = 0; j < d; j++)
        {
            z->filhos[j] = y->filhos[j + meio + 1];
            y->filhos[j + meio + 1] = NULL;
        }
    }

    char *chavePromovida = y->chaves[meio];
    y->chaves[meio] = NULL;

    y->n = d;

    for(int j = pai->n; j >= indice + 1; j--)
    {
        pai->filhos[j + 1] = pai->filhos[j];
    }

    pai->filhos[indice + 1] = z;

    for(int j = pai->n - 1; j >= indice; j--)
    {
        pai->chaves[j + 1] = pai->chaves[j];
    }

    pai->chaves[indice] = chavePromovida;

    pai->n++;
}


//inserção recursiva
void inserirNaoCheio(NoB *p, char *chave)
{
    int i = p->n - 1;

    if(p->folha)
    {
        while(i >= 0 &&
              strcmp(chave,p->chaves[i]) < 0)
        {
            p->chaves[i+1] = p->chaves[i];
            i--;
        }

        p->chaves[i+1] = copiarString(chave);
        p->n++;
    }
    else
    {
        while(i >= 0 &&
              strcmp(chave,p->chaves[i]) < 0)
            i--;

        i++;

        if(p->filhos[i]->n == 2*d)
        {
            splitFilho(p,i);

            if(strcmp(chave,p->chaves[i]) > 0)
                i++;
        }

        inserirNaoCheio(p->filhos[i], chave);
    }
}


// inserção principal
void inserir(char *chave)
{
    if(raiz == NULL)
    {
        raiz = criarNo(1);

        raiz->chaves[0] = copiarString(chave);

        raiz->n = 1;

        return;
    }

    int pos;

    if(buscar(raiz, chave, &pos))
    {
        printf("Chave ja existe.\n");
        return;
    }

    if(raiz->n == 2*d)
    {
        NoB *novaRaiz = criarNo(0);

        novaRaiz->filhos[0] = raiz;

        splitFilho(novaRaiz,0);

        raiz = novaRaiz;
    }

    inserirNaoCheio(raiz,chave);
}


// carregamento do arquivo
void carregarArquivo()
{
    //mudar caminho do arquivo (pokemon_names.txt)
    FILE *arq = fopen("pokemon_names.txt","r");

    if(arq == NULL) 
    {
        printf(
            "Erro ao abrir pokemon_names.txt.\n"
            "Verifique se o arquivo esta no mesmo diretorio do executavel.\n"
        );
        return;
    }
    printf("Arquivo carregado com sucesso!\n");


    char nome[100];

    while(fgets(nome,100,arq))
    {
        nome[strcspn(nome,"\n")] = '\0';

        printf("Inserindo: %s\n", nome);

        inserir(nome);
    }

    fclose(arq);
}


//  menu/main
int main()
{
    do{
        printf("Digite a ordem d: ");
        scanf("%d",&d);

        if(d < 2)
            printf("Valor invalido!\n");

    } while(d < 2);

    carregarArquivo();
    
    int opcao;

    do
    {
        printf("\n");
        printf("// ----- // ----- // ARVORE B // ----- // ----- //\n");
        printf("[1] - Buscar\n");
        printf("[2] - Inserir\n");
        printf("[9] - Finalizar\n");

        scanf("%d",&opcao);

        if(opcao == 1)
        {
            char chave[100];

            printf("Chave: ");
            scanf(" %[^\n]", chave);

            int pos;

            NoB *resp = buscar(raiz,chave,&pos);

            if(resp)
                printf("Encontrada na posicao %d do no.\n",pos);
            else
                printf("Nao encontrada.\n");
        }

        else if(opcao == 2)
        {
            char chave[100];

            printf("Chave: ");
            scanf(" %[^\n]", chave);

            inserir(chave);
        }

    }while(opcao != 9);

    return 0;
}

