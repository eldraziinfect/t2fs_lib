#ifndef __DISK___
#define __DISK___

#include "libs.h"

void init_api_cluster(int tam);
int read_cluster(int sector, unsigned char *buffer);
int write_cluster(int sector, unsigned char *buffer);

#endif
