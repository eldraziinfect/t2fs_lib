#ifndef __UTIL___
#define __UTIL___

#include "libs.h"


int get_root_dir(char* buffer);
void print_dir(struct t2fs_record record);
int cluster_to_sector(int cluster);

#endif

