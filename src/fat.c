#include "../include/libs.h"
#include <math.h>

extern struct t2fs_superbloco super_bloco;

DESCRITOR_FAT descritor_fat;
FAT fat;

int init_fat()
{
    int tam_fat_cluster = (super_bloco.NofSectors - super_bloco.DataSectorStart)/super_bloco.SectorsPerCluster;
    int final_fat = super_bloco.DataSectorStart - 1;
    int inicio_fat = super_bloco.pFATSectorStart;

    descritor_fat.tamanho = tam_fat_cluster;
    descritor_fat.inicio = inicio_fat;
    descritor_fat.fim = final_fat;
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

int set_elemento_fat(int cluster, int value)
{
    if(value == 0x00000001)
        return -1;

    unsigned char* buffer = malloc(TAM_SETOR * sizeof(unsigned char));
    int clusters_por_setor = TAM_SETOR/sizeof(int);
    int quociente = cluster/clusters_por_setor;
    int resto = cluster % clusters_por_setor;

    if(read_sector(descritor_fat.inicio+quociente, buffer))
        return -1;
    memcpy(buffer+(resto*sizeof(int)),&value,sizeof(int));
    if(write_sector(descritor_fat.inicio+quociente, buffer))
        return -1;
    free(buffer);
    return 0;
}

int get_elemento_fat(int cluster)
{
    int result;
    unsigned char* buffer = malloc(TAM_SETOR * sizeof(unsigned char));
    int clusters_por_setor = TAM_SETOR/sizeof(int);
    int quociente = cluster/clusters_por_setor;
    int resto = cluster % clusters_por_setor;

    if(read_sector(descritor_fat.inicio+quociente, buffer))
        return -1;
    memcpy(&result,buffer+(resto*sizeof(int)),sizeof(int));
    free(buffer);
    return result;
}
int test_fat()
{
    int cont_livre=0,cont_ocupado=0,cont_erro=0,cont_erro_intratavel=0;
    int i;

    printf("---------------------------------------------\n");
    printf("Teste da tabela fat: \n");
    for(i = 0; i < descritor_fat.tamanho; i++)
    {
        if(i<20)
            printf("Cluster[%d] = %X\n",i,get_elemento_fat(i));
        switch(get_elemento_fat(i))
        {
        case CLUSTER_LIVRE:
            cont_livre++;
            break;
        case CLUSTER_ERRO_INTRATAVEL:
            cont_erro_intratavel++;
            break;
        case CLUSTER_COM_ERRO:
            cont_erro++;
            break;
        case CLUSTER_EOF:
            cont_ocupado++;
            break;
        default:
            cont_ocupado++;
            break;
        }
    }
    printf("Livres: %d\n",cont_livre);
    printf("Ocupados: %d\n",cont_ocupado);
    printf("Erro: %d\n",cont_erro);
    printf("Erro intratável: %d\n",cont_erro_intratavel);


    int conteudo = 0xA0B0FFA;
    int val = get_next_livre();
    printf("Tentando inserir o valor %X na posicao %d\n",conteudo,val);
    if(set_elemento_fat(val,conteudo) == 0)
    {
        printf("Cluster[%d] : %X\n",val,get_elemento_fat(val));
        printf("Insercao feita com sucesso!\n");
    }
    set_elemento_fat(val,CLUSTER_LIVRE);
    printf("---------------------------------------------\n");

    return 0;
}

int get_next_livre()
{
    int i;
    for(i = 2; i < descritor_fat.tamanho; i++)
    {
        switch(get_elemento_fat(i))
        {
        case CLUSTER_LIVRE:
            return i;
            break;
        }
    }
    return -1;
}





