#include <libpmemobj.h>
#include <sys/signal.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(rect_calc);
POBJ_LAYOUT_ROOT(rect_calc, struct root);
POBJ_LAYOUT_TOID(rect_calc, struct rectangle);
POBJ_LAYOUT_END(rect_calc);

struct root {
  struct rectangle rect;
};

void recover() {
  PMEMobjpool* pop = pmemobj_open(FILE_PATH, POBJ_LAYOUT_NAME(rect_calc));
  PMEMoid root_oid = pmemobj_root(pop, sizeof(struct root));
  struct root* proot = (struct root*)pmemobj_direct(root_oid);

  printf("x: %d, y: %d\n", proot->rect.x, proot->rect.y);

  assert(proot->rect.x == 16);
  assert(proot->rect.y == 16);

  pmemobj_close(pop);
}

void make_error() {
  remove_file();
  PMEMobjpool* pop =
      pmemobj_create(FILE_PATH, POBJ_LAYOUT_NAME(rect_calc), FILE_SIZE, 0666);

  PMEMoid root_oid = pmemobj_root(pop, sizeof(struct root));
  struct root* proot = (struct root*)pmemobj_direct(root_oid);
  proot->rect.x = 10;
  proot->rect.y = 10;
  pmemobj_persist(pop, &proot->rect, sizeof(proot->rect));

  proot->rect.x = 16;
  proot->rect.y = 16;

  BIUBIUBIU();
  // pmemobj_persist(pop, &proot->rect, sizeof(proot->rect));

  pmemobj_close(pop);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s [recover/break]\n", argv[0]);
    return 0;
  }

  if (memcmp(argv[1], "recover", 8) == 0)
    recover();
  else if (memcmp(argv[1], "break", 6) == 0)
    make_error();

  return 0;
}