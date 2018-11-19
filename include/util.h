#ifndef __UTIL___
#define __UTIL___

#include "libs.h"


int get_root_dir(unsigned char* buffer);
void print_dir(struct t2fs_record record);
int cluster_to_sector(int cluster);
int seek_file_in_dir(int sector, char* file_name);
int get_father_dir(int cluster);
int seek_dir_in_dir(int cluster, char* dir_name);
int get_dir_tree(int current_dir_pointer, char *buffer);
int insert_record(int cluster, struct t2fs_record record);
int insert_record2(int cluster, struct t2fs_record r1);
void print_sector_as_dir(int cluster);
#endif

