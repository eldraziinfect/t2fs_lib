#include "../include/libs.h"

extern struct t2fs_superbloco super_bloco;

int cluster_to_sector(int cluster){
    int inicio = super_bloco.DataSectorStart; //cluster 0 -> reservado, 1 -> reservado
    int result = inicio + cluster*super_bloco.SectorsPerCluster;
    return result;
}


void print_dir(struct t2fs_record record)
{
    printf("---------------------------------------------\n");
    printf("Printando diretorio: \n");
    printf("TypeVal: %d\n", record.TypeVal);
    printf("Name : %s\n", record.name);
    printf("BytesFileSize: %d bytes\n", record.bytesFileSize);
    printf("ClustersFileSize: %d clusters\n", record.clustersFileSize);
    printf("FirstCluster: %d\n", record.firstCluster);
    printf("---------------------------------------------\n");
    return;
}

int get_root_dir(char* buffer)
{
    printf("Getting root dir...\n");
    int root_dir = cluster_to_sector(super_bloco.RootDirCluster);//super_bloco.DataSectorStart + super_bloco.RootDirCluster*super_bloco.SectorsPerCluster; //início + offset
    struct t2fs_record record;
    if(read_cluster(root_dir, buffer) == 0)
    {
        memcpy(&record,buffer,sizeof(struct t2fs_record));
        print_dir(record);
        return 0;
    }
    else
    {
        printf("********Unable to get root dir********\n");
        return -1;
    }
}
