#include "../include/libs.h"

void init_tabela_arquivos(DESCRITOR_ARQUIVO tabela[]) {
    int i;
    for(i = 0; i < MAX_FILES; i++){
        tabela[i].handle = -1;
        tabela[i].ocupado = 0;
        strcpy(tabela[i].nome,"");
        tabela[i].record.TypeVal = 0;
        strcpy(tabela[i].record.name,"");
        tabela[i].record.bytesFileSize = -1;
        tabela[i].record.clustersFileSize = -1;
        tabela[i].record.firstCluster = -1;
    }
    printf("Tabela de Descritores de Arquivo Iniciada!\n");
}

int get_descritor_livre(DESCRITOR_ARQUIVO tabela[]){
    int i;
    for(i = 0; i < MAX_FILES; i++){
        if(tabela[i].ocupado == 0)
            return i;
    }
    return -1;
}

int insert_tabela_descritores_de_arquivo(DESCRITOR_ARQUIVO tabela[],struct t2fs_record record, char* pathname){
    int i = get_descritor_livre(tabela);

}





