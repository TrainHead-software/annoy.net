//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

int main(void) {
    const char* filename = "test_hamming_persistence.tree";
    RemoveFileIfExists(filename);

    AnnoyIndexHandle index = BuildBasicHammingIndex();
    Save(index, filename);
    Free(index);

    AnnoyIndexHandle loaded = Index(4, Metric_Hamming);
    assert(loaded != NULL);
    Load(loaded, filename);

    assert(GetNItems(loaded) == 3);
    AssertFloatNear(GetDistance(loaded, 0, 1), 1.0f, 0.0001f);

    float query[] = {1.0f, 1.0f, 0.0f, 0.0f};
    int results[3];
    GetNnsByVector(loaded, query, 3, results);
    assert(results[0] == 2);

    Free(loaded);
    RemoveFileIfExists(filename);
    return 0;
}
