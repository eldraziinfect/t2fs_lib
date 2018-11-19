#include "../include/libs.h"


int tam_cluster;

void init_api_cluster(int tam)
{
    tam_cluster = tam;
}

int write_cluster(int sector, unsigned char *buffer)
{
    int i = 0;
    unsigned char *temp = malloc(TAM_SETOR);
    while(i < tam_cluster)
    {
        memcpy(temp,buffer+i*TAM_SETOR, sizeof(TAM_SETOR));
        if(write_sector(sector+i,temp))
            return -1;
        i++;
    }
    free(temp);
    return 0;
}

int read_cluster(int sector, unsigned char *buffer)
{
    int i = 0;
    while(i < tam_cluster)
    {
        if(read_sector(sector+i,(unsigned char *) buffer+i*TAM_SETOR))
            return -1;
        i++;
    }
    return 0;
}
