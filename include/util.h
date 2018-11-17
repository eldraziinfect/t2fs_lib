#ifndef __UTIL___
#define __UTIL___

#include "libs.h"


int get_root_dir(struct t2fs_superbloco super_bloco, char* buffer);
void print_dir(struct t2fs_record record);
int cluster_to_sector(struct t2fs_superbloco super_bloco, int cluster);

#endif

