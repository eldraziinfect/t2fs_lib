
#ifndef __FILES___
#define __FILES___

#include "libs.h"
typedef struct descritor_arquivo{
    FILE2 handle;
    int ocupado;
    char nome[TAM_NOME_ARQUIVO];
    struct t2fs_record record;
} DESCRITOR_ARQUIVO;
#endif
