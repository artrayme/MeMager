#include "memager.h"
#include "memager_tester.h"
#include "benchmark.h"
#include <stdio.h>

int main() {
  test_memory_init();
  test_memory_alloc();
  test_memory_free();
  test_memory_read_and_write();
  test_fragmentation_perf();
  return 0;
}