#include "../include/libs.h"

void init_tabela_arquivos(DESCRITOR_ARQUIVO tabela[])
{
    int i;
    for(i = 0; i < MAX_FILES; i++)
    {
        tabela[i].current_pointer = 0;
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

int get_descritor_livre(DESCRITOR_ARQUIVO tabela[])
{
    int i;
    for(i = 0; i < MAX_FILES; i++)
    {
        if(tabela[i].ocupado == 0)
            return i;
    }
    return -1;
}

int insert_tabela_descritores_de_arquivo(DESCRITOR_ARQUIVO tabela[],struct t2fs_record record, char* pathname)
{
    if(get_descritor(tabela,pathname) != -1)
        return get_descritor(tabela,pathname);
    int i = get_descritor_livre(tabela);
    if(i == -1)
        return i;
    tabela[i].ocupado = 1;
    tabela[i].current_pointer = 0;
    strcpy(tabela[i].nome,pathname);
    memcpy(&tabela[i].record, &record, sizeof(struct t2fs_record));
    return i;

}

int remove_descritor(DESCRITOR_ARQUIVO tabela[], int i)
{
    if(tabela[i].ocupado == 0)
        return -1;
    tabela[i].current_pointer = 0;
    tabela[i].ocupado = 0;
    strcpy(tabela[i].nome,"");
    tabela[i].record.TypeVal = 0;
    strcpy(tabela[i].record.name,"");
    tabela[i].record.bytesFileSize = -1;
    tabela[i].record.clustersFileSize = -1;
    tabela[i].record.firstCluster = -1;
    return 0;
}

int get_descritor(DESCRITOR_ARQUIVO tabela[], char nome[])
{
    int i = 0;
    for(i = 0; i < MAX_FILES; i++)
        if(strcmp(tabela[i].nome,nome)==0)
            return i;
    return -1;
}

/** Função que atualiza o descritor
*   TODO:
*       verificar se o descritor está na tabela;
*       atualizá-lo.
*/

int atualiza_descritor(DESCRITOR_ARQUIVO tabela[], int i)
{
    return 0;
}


int move_pointer(DESCRITOR_ARQUIVO tabela[], int handle, int offset)
{
    if(tabela[handle].ocupado!=0)
    {
        tabela[handle].current_pointer += offset;
        return 0;
    }
    else
    {
        return -1;
    }
}

int get_file_pointer(DESCRITOR_ARQUIVO tabela[], int handle)
{
    if(tabela[handle].ocupado!=0)
    {
        return tabela[handle].current_pointer;
    }
    else
    {
        return -1;
    }
}

