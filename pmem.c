#include <libpmem.h>
#include "common.h"

struct root {
  struct rectangle rect;
};

int main(void) {
  char* pmem_addr;
  size_t mapped_len;
  int is_pmem;

  remove_file();
  pmem_addr = (char*)pmem_map_file(FILE_PATH, FILE_SIZE,
                                   PMEM_FILE_CREATE | PMEM_FILE_EXCL, 0666,
                                   &mapped_len, &is_pmem);
  printf("is_pmem: %d, mapped_len: %ld\n", is_pmem, mapped_len);

  struct root* root = (struct root*)pmem_addr;

  TIMER_INIT;

  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    root->rect.x = 10;
    root->rect.y = 10;
    if (is_pmem)
      pmem_persist(&root->rect.x, sizeof(root->rect.x));
    else
      pmem_msync(&root->rect.x, sizeof(root->rect.x));

    if (is_pmem)
      pmem_persist(&root->rect.y, sizeof(root->rect.y));
    else
      pmem_msync(&root->rect.y, sizeof(root->rect.y));
  }
  TIMER_STOP;

  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    struct rectangle tmp;
    tmp.x = 10;
    tmp.y = 10;
    pmem_memcpy_persist(&root->rect, &tmp, sizeof(tmp));
  }
  TIMER_STOP;
}