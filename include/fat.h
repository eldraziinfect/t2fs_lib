
#ifndef __FAT___
#define __FAT___

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
//get_elemento_fat(int i);
#endif
