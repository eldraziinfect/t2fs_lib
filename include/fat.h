
#ifndef __FAT___
#define __FAT___

#define CLUSTER_LIVRE 0x00000000
#define CLUSTER_COM_ERRO 0xFFFFFFFE
#define CLUSTER_EOF 0xFFFFFFFF
#define CLUSTER_ERRO_INTRATAVEL 0x00000001

#include "libs.h"
typedef struct descritor_fat
{
    int tamanho;
    int inicio;
    int fim;
} DESCRITOR_FAT;

typedef struct fat
{
    int* tabela;
} FAT;
int init_fat();
int get_tamanho_fat();
int get_inicio_fat();
int get_final_fat();
void print_fat();
int set_elemento_fat(int cluster, int value);
int get_elemento_fat(int cluster);
int test_fat();
int get_next_livre();

#endif
