#ifndef _FS_H
#define _FS_H

#include <lib.h>
#include <stdlib.h>

#define MAX_FILE_ENTRIES 16
#define MAX_FILE_NAME_LEN 32

typedef void (*fs_entry_point_t)(void);

typedef struct {
  char name[MAX_FILE_NAME_LEN];
  fs_entry_point_t entry_point;
} fs_entry_t;

int fs_load(const char *name, fs_entry_point_t entry_point);
fs_entry_point_t fs_get_entry(const char *name);

#endif // _FS_H
