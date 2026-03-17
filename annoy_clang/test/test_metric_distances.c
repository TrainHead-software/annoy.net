//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

static void TestEuclideanDistance(void) {
    AnnoyIndexHandle index = BuildBasicEuclideanIndex();
    AssertFloatNear(GetDistance(index, 0, 1), 1.0f, 0.0001f);
    AssertFloatNear(GetDistance(index, 1, 2), 2.2360679f, 0.0001f);
    Free(index);
}

static void TestManhattanDistance(void) {
    AnnoyIndexHandle index = Index(2, Metric_Manhattan);
    assert(index != NULL);

    float v0[] = {0.0f, 0.0f};
    float v1[] = {1.0f, 2.0f};

    AddItem(index, 0, v0);
    AddItem(index, 1, v1);
    Build(index, 10);

    AssertFloatNear(GetDistance(index, 0, 1), 3.0f, 0.0001f);
    Free(index);
}

static void TestDotProductDistance(void) {
    AnnoyIndexHandle index = Index(2, Metric_DotProduct);
    assert(index != NULL);

    float v0[] = {0.0f, 1.0f};
    float v1[] = {1.0f, 1.0f};
    float v2[] = {0.0f, 0.0f};

    AddItem(index, 0, v0);
    AddItem(index, 1, v1);
    AddItem(index, 2, v2);
    Build(index, 10);

    AssertFloatNear(GetDistance(index, 0, 1), 1.0f, 0.0001f);
    AssertFloatNear(GetDistance(index, 1, 2), 0.0f, 0.0001f);
    Free(index);
}

int main(void) {
    TestEuclideanDistance();
    TestManhattanDistance();
    TestDotProductDistance();
    return 0;
}
