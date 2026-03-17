//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

int main(void) {
    const char* filename = "test_angular_persistence.tree";
    RemoveFileIfExists(filename);

    AnnoyIndexHandle index = BuildBasicAngularIndex();
    Save(index, filename);
    Free(index);

    AnnoyIndexHandle loaded = Index(3, Metric_Angular);
    assert(loaded != NULL);
    Load(loaded, filename);

    assert(GetNItems(loaded) == 3);
    assert(GetNTrees(loaded) > 0);

    int by_item[3];
    GetNnsByItem(loaded, 0, 3, -1, by_item);
    assert(by_item[0] == 0);

    Free(loaded);
    RemoveFileIfExists(filename);
    return 0;
}
