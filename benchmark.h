//
// Created by artrayme on 11/17/21.
//

#ifndef MEMAGER__BENCHMARK_H_
#define MEMAGER__BENCHMARK_H_

#endif//MEMAGER__BENCHMARK_H_

#include <time.h>

#define _T_START clock_t t_start = clock(), t_stop
#define _T_STOP t_stop = clock()
#define _T_DIFF (double)(t_stop - t_start) / CLOCKS_PER_SEC

void test_fragmentation_perf();
