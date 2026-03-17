//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

static void FillDeterministicVector(int seed, float* vector, int dimensions) {
    for (int i = 0; i < dimensions; ++i) {
        vector[i] = (float)(((seed + 1) * (i + 3)) % 17) / 17.0f;
    }
}

static AnnoyIndexHandle BuildSeededAngularIndex(int seed) {
    AnnoyIndexHandle index = Index(8, Metric_Angular);
    assert(index != NULL);

    SetSeed(index, seed);
    SetVerbose(index, 1);
    SetVerbose(index, 0);

    for (int item = 0; item < 64; ++item) {
        float vector[8];
        FillDeterministicVector(item, vector, 8);
        AddItem(index, item, vector);
    }

    Build(index, 10);
    return index;
}

static void TestSeedProducesStableResults(void) {
    AnnoyIndexHandle first = BuildSeededAngularIndex(42);
    AnnoyIndexHandle second = BuildSeededAngularIndex(42);

    for (int query_index = 0; query_index < 5; ++query_index) {
        float query[8];
        int first_results[10];
        int second_results[10];

        FillDeterministicVector(query_index + 100, query, 8);
        GetNnsByVector(first, query, 10, first_results);
        GetNnsByVector(second, query, 10, second_results);
        AssertIntArrayEquals(first_results, second_results, 10);
    }

    Free(first);
    Free(second);
}

static void TestOnDiskBuildRoundTrip(void) {
    const char* filename = "test_on_disk_build.tree";
    RemoveFileIfExists(filename);

    AnnoyIndexHandle index = Index(2, Metric_Euclidean);
    assert(index != NULL);
    assert(OnDiskBuild(index, filename) == 1);

    AddBasicEuclideanItems(index);
    Build(index, 10);

    float query[] = {4.0f, 4.0f};
    int expected[] = {2, 1, 0};
    int results[3];
    GetNnsByVector(index, query, 3, results);
    AssertIntArrayEquals(results, expected, 3);
    Free(index);

    AnnoyIndexHandle loaded = Index(2, Metric_Euclidean);
    assert(loaded != NULL);
    Load(loaded, filename);
    GetNnsByVector(loaded, query, 3, results);
    AssertIntArrayEquals(results, expected, 3);
    Free(loaded);

    RemoveFileIfExists(filename);
}

int main(void) {
    TestSeedProducesStableResults();
    TestOnDiskBuildRoundTrip();
    return 0;
}
