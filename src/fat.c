#include "../include/libs.h"
/*
typedef struct descritor_fat
{
    int tamanho;
    int inicio;
} DESCRITOR_FAT;


typedef struct fat
{
    int tabela[];
} FAT;

*/

DESCRITOR_FAT descritor_fat;
FAT fat;

int init_fat(struct t2fs_superbloco super_bloco){
    int tam_fat_cluster = (super_bloco.NofSectors - super_bloco.DataSectorStart)/super_bloco.SectorsPerCluster;
    int final_fat = super_bloco.DataSectorStart - 1;
    int inicio_fat = super_bloco.pFATSectorStart;

    descritor_fat.tamanho = tam_fat_cluster;
    descritor_fat.inicio = inicio_fat;
    descritor_fat.fim = final_fat;
    fat.tabela = malloc(tam_fat_cluster*sizeof(int));
    return 0;
}

int get_tamanho_fat()
{
    return descritor_fat.tamanho;
}

int get_inicio_fat()
{
    return descritor_fat.inicio; // setor lógico.
}

int get_final_fat()
{
    return descritor_fat.fim; // setor lógico.
}

void print_fat()
{
    printf("---------------------------------------------\n");
    printf("FAT\n");
    printf("Tamanho: %d (entradas)\n", get_tamanho_fat());
    printf("Inicio: %d (setor logico)\n", get_inicio_fat());
    printf("Fim: %d\n", get_final_fat());
    printf("---------------------------------------------\n");

}








