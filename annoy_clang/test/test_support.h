//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#ifndef ANNOY_C_TEST_SUPPORT_H
#define ANNOY_C_TEST_SUPPORT_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/annoy_c_wrapper.h"

static inline void AssertFloatNear(float actual, float expected, float tolerance) {
    assert(fabsf(actual - expected) <= tolerance);
}

static inline void AssertIntArrayEquals(const int* actual, const int* expected, int count) {
    for (int i = 0; i < count; ++i) {
        assert(actual[i] == expected[i]);
    }
}

static inline void AssertFloatArrayEquals(const float* actual, const float* expected, int count, float tolerance) {
    for (int i = 0; i < count; ++i) {
        AssertFloatNear(actual[i], expected[i], tolerance);
    }
}

static inline void RemoveFileIfExists(const char* filename) {
    remove(filename);
}

static inline void AddBasicAngularItems(AnnoyIndexHandle index) {
    float v0[] = {1.0f, 0.0f, 0.0f};
    float v1[] = {0.0f, 1.0f, 0.0f};
    float v2[] = {0.0f, 0.0f, 1.0f};

    AddItem(index, 0, v0);
    AddItem(index, 1, v1);
    AddItem(index, 2, v2);
}

static inline AnnoyIndexHandle BuildBasicAngularIndex(void) {
    AnnoyIndexHandle index = Index(3, Metric_Angular);
    assert(index != NULL);
    AddBasicAngularItems(index);
    Build(index, 10);
    return index;
}

static inline void AddBasicEuclideanItems(AnnoyIndexHandle index) {
    float v0[] = {0.0f, 0.0f};
    float v1[] = {1.0f, 0.0f};
    float v2[] = {0.0f, 2.0f};

    AddItem(index, 0, v0);
    AddItem(index, 1, v1);
    AddItem(index, 2, v2);
}

static inline AnnoyIndexHandle BuildBasicEuclideanIndex(void) {
    AnnoyIndexHandle index = Index(2, Metric_Euclidean);
    assert(index != NULL);
    AddBasicEuclideanItems(index);
    Build(index, 10);
    return index;
}

static inline void AddBasicHammingItems(AnnoyIndexHandle index) {
    float h0[] = {0.0f, 0.0f, 0.0f, 0.0f};
    float h1[] = {1.0f, 0.0f, 0.0f, 0.0f};
    float h2[] = {1.0f, 1.0f, 0.0f, 0.0f};

    AddItem(index, 0, h0);
    AddItem(index, 1, h1);
    AddItem(index, 2, h2);
}

static inline AnnoyIndexHandle BuildBasicHammingIndex(void) {
    AnnoyIndexHandle index = Index(4, Metric_Hamming);
    assert(index != NULL);
    AddBasicHammingItems(index);
    Build(index, 10);
    return index;
}

#endif // ANNOY_C_TEST_SUPPORT_H
