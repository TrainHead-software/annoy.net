//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

int main(void) {
    AnnoyIndexHandle index = Index(3, Metric_Angular);
    assert(index != NULL);

    SetVerbose(index, 1);
    SetVerbose(index, 0);
    AddBasicAngularItems(index);

    assert(GetNItems(index) == 3);

    Build(index, 10);

    assert(GetNItems(index) == 3);
    assert(GetNTrees(index) > 0);

    Free(index);
    return 0;
}
