
#ifndef __FILES___
#define __FILES___

#include "libs.h"
typedef struct descritor_arquivo{
    int current_pointer;
    int ocupado;
    char nome[TAM_NOME_ARQUIVO];
    struct t2fs_record record;
} DESCRITOR_ARQUIVO;

void init_tabela_arquivos(DESCRITOR_ARQUIVO tabela[]);
int get_descritor_livre(DESCRITOR_ARQUIVO tabela[]);
int insert_tabela_descritores_de_arquivo(DESCRITOR_ARQUIVO tabela[],struct t2fs_record record, char* pathname);
int remove_descritor(DESCRITOR_ARQUIVO tabela[], int i);
int get_descritor(DESCRITOR_ARQUIVO tabela[], char nome[]);
int atualiza_descritor(DESCRITOR_ARQUIVO tabela[], int i);
#endif
