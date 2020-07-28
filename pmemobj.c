#include <libpmemobj.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(rect_calc);
POBJ_LAYOUT_ROOT(rect_calc, struct root);
POBJ_LAYOUT_TOID(rect_calc, struct rectangle);
POBJ_LAYOUT_END(rect_calc);

struct root {
  struct rectangle rect;
};

int main(void) {
  remove_file();
  PMEMobjpool* pop =
      pmemobj_create(FILE_PATH, POBJ_LAYOUT_NAME(rect_calc), FILE_SIZE, 0666);

  PMEMoid root_oid = pmemobj_root(pop, sizeof(struct root));
  struct root* proot = (struct root*)pmemobj_direct(root_oid);

  TIMER_INIT;

  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    proot->rect.x = 10;
    pmemobj_persist(pop, &proot->rect.x, sizeof(proot->rect.x));
    proot->rect.y = 10;
    pmemobj_persist(pop, &proot->rect.y, sizeof(proot->rect.y));
  }
  TIMER_STOP;

  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    struct rectangle tmp;
    tmp.x = 10;
    tmp.y = 10;
    pmemobj_memcpy_persist(pop, &proot->rect, &tmp, sizeof(tmp));
  }
  TIMER_STOP;

  return 0;
}