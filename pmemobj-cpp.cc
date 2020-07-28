#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_atomic.hpp>
#include <iostream>
#include "common.h"

using namespace pmem::obj;

struct p_rectangle {
  p_rectangle(int x, int y): x(x), y(y) {}
  p<int> x;
  p<int> y;
};

struct root {
  persistent_ptr<rectangle> rect;
  persistent_ptr<p_rectangle> p_rect;
};

int main(void) {
  remove_file();
  auto pop = pool<root>::create(FILE_PATH, "rect_calc", FILE_SIZE, 0666);
  auto proot = pop.root();

  TIMER_INIT;

  transaction::run(pop, [&](){proot->rect = make_persistent<rectangle>(1, 1);});
  transaction::run(pop, [&](){proot->p_rect = make_persistent<p_rectangle>(1, 1);});

  printf("persist()\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    proot->rect->x = 10;
    proot->rect->y = 10;
    proot->rect.persist();
  }
  TIMER_STOP;

  printf("memcpy_persist()\n");
  rectangle tmp(10, 10);
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    pop.memcpy_persist(proot->rect.get(), &tmp, sizeof(tmp));
  }
  TIMER_STOP;

  printf("memcpy_persist()\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    rectangle tmp1(10, 10);
    pop.memcpy_persist(proot->rect.get(), &tmp1, sizeof(tmp1));
  }
  TIMER_STOP;

  printf("transaction run\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    transaction::run(pop, [&](){
      proot->rect->x = 10;
      proot->rect->y = 10;
      proot->rect.persist();
    });
  }
  TIMER_STOP;

  printf("transaction run snapshot\n");
  TIMER_START;
  for (int i = 0; i < TEST_SIZE; ++i) {
    transaction::run(pop, [&](){
      proot->p_rect->x = 10;
      proot->p_rect->y = 10;
    });
  }
  TIMER_STOP;

  return 0;
}