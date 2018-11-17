#ifndef __FILES___
#define __FILES___

#include "libs.h"

void init_api_cluster(int tam);
int read_cluster(int sector, char *buffer);
int write_cluster(int sector, char *buffer);

#endif
