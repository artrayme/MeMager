//#include "memager_tester.c"
#include "memager_tester.h"
#include <stdio.h>
#include "memager.h"

int main() {
//  test_memory_init();
  test_memory_alloc();
  test_memory_free();
  return 0;
}