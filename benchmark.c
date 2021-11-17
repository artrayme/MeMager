//
// Created by artrayme on 11/17/21.
//

#include "benchmark.h"
#include "memager.h"
#include <stdio.h>

void test_fragmentation_perf() {
  printf("\nfragmentation test:\n\
          \rSIZE\t|NUMBER\t|TIME\n");
  for (size_t s_size = 1; s_size <= 101; s_size += 10) {
    for (size_t s_number = 1; s_number <= 101; s_number += 10) {
      _T_START;
      init_memory(s_size, s_number);
      for (int i = 0; i < s_number; i++) {
        ptr ptr1 = alloc_ptr(s_size-1);
      }
      init_memory(1,1);
      _T_STOP;

      printf("%ld\t|%ld\t|%f\n", s_size, s_number, _T_DIFF);
    }
  }
}