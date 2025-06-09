// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <fs.h>

static fs_entry_t fs_entries[MAX_FILE_ENTRIES];
static int fs_entry_count = 0;

int fs_load(const char *name, fs_entry_point_t entry_point) {
  if (fs_entry_count >= MAX_FILE_ENTRIES || str_len(name) >= MAX_FILE_NAME_LEN)
    return -1;
  for (int i = 0; i < fs_entry_count; ++i) {
    if (str_cmp(fs_entries[i].name, name) == 0)
      return -1;
  }
  str_ncpy(fs_entries[fs_entry_count].name, name, MAX_FILE_NAME_LEN - 1);
  fs_entries[fs_entry_count].name[MAX_FILE_NAME_LEN - 1] = '\0';
  fs_entries[fs_entry_count].entry_point = entry_point;
  fs_entry_count++;
  return 0;
}

fs_entry_point_t fs_get_entry(const char *name) {
  for (int i = 0; i < fs_entry_count; ++i) {
    if (str_cmp(fs_entries[i].name, name) == 0)
      return fs_entries[i].entry_point;
  }
  return NULL;
}

int fs_rm(const char *name) {
  for (int i = 0; i < fs_entry_count; ++i) {
    if (str_cmp(fs_entries[i].name, name) == 0) {
      for (int j = i; j < fs_entry_count - 1; ++j) {
        fs_entries[j] = fs_entries[j + 1];
      }
      fs_entry_count--;
      return 0;
    }
  }
  return -1;
}

int fs_list_programs(char buffer[][MAX_FILE_NAME_LEN], int max_count,
                     int *out_count) {
  int count = 0;
  for (int i = 0; i < fs_entry_count && count < max_count; ++i) {
    str_ncpy(buffer[count], fs_entries[i].name, MAX_FILE_NAME_LEN - 1);
    buffer[count][MAX_FILE_NAME_LEN - 1] = '\0';
    count++;
  }
  if (out_count) {
    *out_count = count;
  }
  return 0;
}
