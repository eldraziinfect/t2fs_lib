#include "../include/libs.h"

extern struct t2fs_superbloco super_bloco;

int cluster_to_sector(int cluster)
{
    int inicio = super_bloco.DataSectorStart; //cluster 0 -> reservado, 1 -> reservado
    int result = inicio + cluster*super_bloco.SectorsPerCluster;
    return result;
}


void print_dir(struct t2fs_record record)
{
    if(record.TypeVal != 0)
    {
        printf("---------------------------------------------\n");
        printf("Printando diretorio: \n");
        printf("TypeVal: %d\n", record.TypeVal);
        printf("Name : %s\n", record.name);
        printf("BytesFileSize: %d bytes\n", record.bytesFileSize);
        printf("ClustersFileSize: %d clusters\n", record.clustersFileSize);
        printf("FirstCluster: %d\n", record.firstCluster);
        printf("---------------------------------------------\n");
    }
    return;
}

void print_sector_as_dir(int cluster)
{
    printf("\n\nSetor: %d\n",cluster);
    int sector = cluster_to_sector(cluster);
    unsigned char* buffer = malloc(TAM_SETOR);
    struct t2fs_record iterator;
    int i,j;
    for(i = 0; i < super_bloco.SectorsPerCluster; i++)
    {
        read_sector(i+sector,buffer);
        j = 0;
        while(j < TAM_SETOR/sizeof(struct t2fs_record))
        {
            memcpy(&iterator,buffer+(j*sizeof(struct t2fs_record)), sizeof(struct t2fs_record));
            print_dir(iterator);
            j++;
        }
    }
    free(buffer);
    return;
}

int is_deletable_dir(int cluster)
{
    int irregular = 0;
    int sector = cluster_to_sector(cluster);
    unsigned char* buffer = malloc(TAM_SETOR);
    struct t2fs_record iterator;
    int i,j;
    for(i = 0; i < super_bloco.SectorsPerCluster; i++)
    {
        read_sector(i+sector,buffer);
        j = 0;
        while(j < TAM_SETOR/sizeof(struct t2fs_record))
        {
            memcpy(&iterator,buffer+(j*sizeof(struct t2fs_record)), sizeof(struct t2fs_record));
            if(strcmp(iterator.name,".") != 0){
                if(strcmp(iterator.name,"..") != 0)
                {
                    if(iterator.bytesFileSize != 0)
                    {
                    irregular = -1;
                    break;
                    }
                }
            }
            j++;
        }
    }
    free(buffer);
    return irregular;
}

int get_root_dir(unsigned char* buffer)
{
    printf("Getting root dir...\n");
    int root_dir = cluster_to_sector(super_bloco.RootDirCluster);
    struct t2fs_record record;
    if(read_cluster(root_dir, buffer) == 0)
    {
        memcpy(&record,buffer,sizeof(struct t2fs_record));
        return 0;
    }
    else
    {
        printf("********Unable to get root dir********\n");
        return -1;
    }
}

int seek_dir_in_dir(int cluster, char* dir_name)
{
    int sector = cluster_to_sector(cluster);
    unsigned char* buffer = malloc(TAM_SETOR);
    struct t2fs_record iterator;
    int retorno = -1;
    int i,j;
    for(i = 0; i < super_bloco.SectorsPerCluster; i++)
    {
        read_sector(i+sector,buffer);
        j = 0;
        while(j < TAM_SETOR/sizeof(struct t2fs_record))
        {
            memcpy(&iterator,buffer+(j*sizeof(struct t2fs_record)), sizeof(struct t2fs_record));
            if(strcmp(iterator.name, dir_name) == 0)
            {
                if(iterator.TypeVal == TYPEVAL_DIRETORIO)
                {
                    retorno = iterator.firstCluster;
                    break;
                }
            }
            j++;
        }
    }
    free(buffer);
    //printf("\nSeek Dir In Dir\nProcurei por: %s\nCluster : %d\nNao achei\n",dir_name,cluster);
    return retorno;
}
int seek_file_in_dir(int cluster, char* file_name)
{
    struct t2fs_record iterator;
    unsigned char* buffer = malloc(super_bloco.SectorsPerCluster*TAM_SETOR);

    int parsed_cluster = cluster_to_sector(cluster);
    read_cluster(parsed_cluster, buffer);

    int i=0;
    while( i < ((super_bloco.SectorsPerCluster*TAM_SETOR)/sizeof(struct t2fs_record)))
    {
        memcpy(&iterator,buffer+i*sizeof(struct t2fs_record), sizeof(struct t2fs_record));
        print_dir(iterator);
        if(strncmp(iterator.name, file_name,strlen(file_name)) == 0)
        {
            free(buffer);
            return iterator.firstCluster;
        }
        i++;
    }
    free(buffer);
    return -1;
}

int free_dir_entry(int cluster, char *dir_name)
{
    int sector = cluster_to_sector(cluster);
    unsigned char* buffer = malloc(TAM_SETOR);
    struct t2fs_record iterator;
    int i,j;
    for(i = 0; i < super_bloco.SectorsPerCluster; i++)
    {
        read_sector(i+sector,buffer);
        j = 0;
        while(j < TAM_SETOR/sizeof(struct t2fs_record))
        {
            memcpy(&iterator,buffer+(j*sizeof(struct t2fs_record)), sizeof(struct t2fs_record));
            if(strcmp(iterator.name,dir_name) == 0)
            {
                iterator.TypeVal = TYPEVAL_INVALIDO;
                memcpy(buffer+(j*sizeof(struct t2fs_record)),&iterator, sizeof(struct t2fs_record));
                write_sector(i+sector,buffer);
                free(buffer);
                return 0;
            }
            j++;
        }
    }
    free(buffer);
    return -1;
}


int seek_dir_by_first_cluster(int cluster, int first_cluster, char *dir_name)
{
    int sector = cluster_to_sector(cluster);
    unsigned char* buffer = malloc(TAM_SETOR);
    struct t2fs_record iterator;
    int i,j;
    for(i = 0; i < super_bloco.SectorsPerCluster; i++)
    {
        read_sector(i+sector,buffer);
        j = 0;
        while(j < TAM_SETOR/sizeof(struct t2fs_record))
        {
            memcpy(&iterator,buffer+(j*sizeof(struct t2fs_record)), sizeof(struct t2fs_record));
            if(iterator.firstCluster == first_cluster)
            {
                strcpy(dir_name,iterator.name);
                free(buffer);
                return 0;
            }
            j++;
        }
    }
    free(buffer);
    return -1;
}

int get_father_dir(int cluster)
{
    return seek_dir_in_dir(cluster,"..");
}

int get_dir_name(int cluster, char *buffer)
{
    seek_dir_by_first_cluster(get_father_dir(cluster),cluster,buffer);
    return 0;
}

int get_dir_tree(int current_dir_pointer, char* buffer)
{
    if(current_dir_pointer == super_bloco.RootDirCluster)
    {
        strcpy(buffer,"/");
        return 0;
    }
    get_dir_name(current_dir_pointer,buffer);
    char* aux = malloc(TAM_NOME_ARQUIVO+1);
    int dir_pai = seek_dir_in_dir(current_dir_pointer,"..");

    if(dir_pai == -1)
    {
        free(aux);
        return -1;
    }

    while(dir_pai != super_bloco.RootDirCluster)
    {
        prepend(buffer,"/");
        get_dir_name(dir_pai,aux);
        prepend(buffer,aux);
        dir_pai = seek_dir_in_dir(dir_pai,"..");
        strcpy(aux,"");
    }
    prepend(buffer,"/");
    free(aux);
    return 0;
}

void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
}

int get_free_record(int cluster)
{

    return 0;
}


int insert_record(int cluster, struct t2fs_record r1)
{
    int sector = cluster_to_sector(cluster);
    unsigned char* buffer = malloc(TAM_SETOR);
    struct t2fs_record iterator;
    int i,j;
    for(i = 0; i < super_bloco.SectorsPerCluster; i++)
    {
        read_sector(i+sector,buffer);
        j = 0;
        while(j < TAM_SETOR/sizeof(struct t2fs_record))
        {
            memcpy(&iterator,buffer+(j*sizeof(struct t2fs_record)), sizeof(struct t2fs_record));
            if(iterator.TypeVal == TYPEVAL_INVALIDO)
            {
                memcpy(buffer+(j*sizeof(struct t2fs_record)),&r1, sizeof(struct t2fs_record));
                write_sector(i+sector,buffer);
                free(buffer);
                return 0;
            }
            j++;
        }
    }
    free(buffer);
    return -1;
}







