#include <libpmemobj.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(rect_calc);
POBJ_LAYOUT_ROOT(rect_calc, struct root);
POBJ_LAYOUT_TOID(rect_calc, struct rectangle);
POBJ_LAYOUT_END(rect_calc);

struct root {
  TOID(struct rectangle) rect;
};

int main(void) {
  remove_file();
  PMEMobjpool* pop =
      pmemobj_create(FILE_PATH, POBJ_LAYOUT_NAME(rect_calc), FILE_SIZE, 0666);

  TOID(struct root) root = POBJ_ROOT(pop, struct root);

  TX_BEGIN(pop) {
    TX_ADD(root);
    TOID(struct rectangle) rect = TX_NEW(struct rectangle);
    D_RW(rect)->x = 5;
    D_RW(rect)->y = 10;
    D_RW(root)->rect = rect;
  }
  TX_ONCOMMIT { printf("commit\n"); }
  TX_ONABORT { printf("on abort\n"); }
  TX_FINALLY { printf("final\n"); }
  TX_END

  TIMER_INIT;

  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    TX_BEGIN(pop) {
      TX_ADD(D_RW(root)->rect);
      D_RW(D_RW(root)->rect)->x = 10;
      D_RW(D_RW(root)->rect)->y = 10;
    }
    TX_END
  }
  TIMER_STOP;

  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    TX_BEGIN(pop) {
      TX_SET(D_RW(root)->rect, x, 10);
      TX_SET(D_RW(root)->rect, y, 10);
    }
    TX_END
  }
  TIMER_STOP;

  return 0;
}