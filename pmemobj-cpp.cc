#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_atomic.hpp>
#include <iostream>
#include "common.h"

using namespace pmem::obj;

struct root {
  persistent_ptr<rectangle> mdata;
};

int main(void) {
  remove_file();
  auto pop = pool<root>::create(FILE_PATH, "rect_calc", FILE_SIZE * 256, 0666);
  auto proot = pop.root();

  TIMER_INIT;

  transaction::run(pop, [&](){proot->mdata = make_persistent<rectangle>(1, 1);});

  printf("persist()\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    proot->mdata->x = 10;
    proot->mdata->y = 10;
    proot->mdata.persist();
  }
  TIMER_STOP;

  printf("make_persistent\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    transaction::run(pop, [&](){proot->mdata = make_persistent<rectangle>(5, 5);});
  }
  TIMER_STOP;

  printf("transaction run\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    transaction::run(pop, [&](){
      proot->mdata->x = 10;
      proot->mdata->y = 10;
    });
  }
  TIMER_STOP;

  printf("transaction run persist\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    transaction::run(pop, [&](){
      proot->mdata->x = 10;
      proot->mdata->y = 10;
      proot->mdata.persist();
    });
  }
  TIMER_STOP;

  printf("make_persistent_atomic\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    transaction::run(pop, [&](){make_persistent_atomic<rectangle>(pop, proot->mdata, 10, 10);});
  }
  TIMER_STOP;

  return 0;
}