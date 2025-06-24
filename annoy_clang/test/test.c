//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/annoy_c_wrapper.h"

int main() {
    // A = AnnoyIndex(3, 'angular')
    AnnoyIndexHandle a = Index(3, Metric_Angular);

    // a.add_item(0, [1, 0, 0])
    float v0[] = {1.0f, 0.0f, 0.0f};
    AddItem(a, 0, v0);

    // a.add_item(1, [0, 1, 0])
    float v1[] = {0.0f, 1.0f, 0.0f};
    AddItem(a, 1, v1);

    // a.add_item(2, [0, 0, 1])
    float v2[] = {0.0f, 0.0f, 1.0f};
    AddItem(a, 2, v2);

    // a.build(-1)
    Build(a, -1);

    // a.save("test.tree")
    Save(a, "test.tree");

    // b = AnnoyIndex(3)
    AnnoyIndexHandle b = Index(3, Metric_Angular);

    // b.load("test.tree")
    Load(b, "test.tree");

    // print(b.get_nns_by_item(0, 100))
    int results_by_item[100];
    GetNnsByItem(b, 0, 100, -1, results_by_item);
    printf("Nearest by item 0:\n");
    for (int i = 0; i < 3; i++) {
        printf("  %d\n", results_by_item[i]);
    }

    // print(b.get_nns_by_vector([1.0, 0.5, 0.5], 100))
    float query[] = {1.0f, 0.5f, 0.5f};
    int results_by_vector[100];
    GetNnsByVector(b, query, 100, results_by_vector);
    printf("Nearest by vector [1.0, 0.5, 0.5]:\n");
    for (int i = 0; i < 3; i++) {
        printf("  %d\n", results_by_vector[i]);
    }

    // cleanup
    Free(a);
    Free(b);
    return 0;
}
