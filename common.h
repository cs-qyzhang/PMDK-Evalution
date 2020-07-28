#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define FILE_PATH "/mnt/pmem0/persistent"

#define remove_file() system("rm " FILE_PATH)

#define TIMER_INIT struct timeval stop, start

#define TIMER_START             \
  do {                          \
    gettimeofday(&start, NULL); \
  } while (0)

#define TIMER_STOP                                                   \
  do {                                                               \
    gettimeofday(&stop, NULL);                                       \
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + \
                                stop.tv_usec - start.tv_usec);       \
  } while (0)

#define TEST_SIZE 10000000

#define FILE_SIZE ((size_t)(1024 * 1024 * 8)) /* 8 MiB */

struct rectangle {
  int x;
  int y;
};