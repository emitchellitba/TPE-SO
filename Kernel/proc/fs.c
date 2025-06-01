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
