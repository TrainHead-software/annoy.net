//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

int main(void) {
    assert(Metric_Angular == 0);
    assert(Metric_Euclidean == 1);
    assert(Metric_Manhattan == 2);
    assert(Metric_DotProduct == 3);
    assert(Metric_Hamming == 4);
    return 0;
}
