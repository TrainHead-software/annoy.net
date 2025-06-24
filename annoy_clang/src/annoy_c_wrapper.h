//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#ifndef ANNOY_C_WRAPPER_H
#define ANNOY_C_WRAPPER_H
#include "annoy_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Metric_Angular,
    Metric_Euclidean,
    Metric_Manhattan,
    Metric_DotProduct,
    //Metric_Hamming
} AnnoyMetric;

typedef void* AnnoyIndexHandle;

ANNOY_API AnnoyIndexHandle Index(int f, AnnoyMetric metric);
void ANNOY_API AddItem(AnnoyIndexHandle handle, int item, float* vector);
void ANNOY_API Build(AnnoyIndexHandle handle, int n_trees);
void ANNOY_API Save(AnnoyIndexHandle handle, const char* filename);
void ANNOY_API Load(AnnoyIndexHandle handle, const char* filename);
void ANNOY_API GetNnsByVector(AnnoyIndexHandle handle, float* vector, int n, int* result);
void ANNOY_API Free(AnnoyIndexHandle handle);
float ANNOY_API GetDistance(AnnoyIndexHandle handle, int i, int j);
void ANNOY_API GetNnsByItem(AnnoyIndexHandle handle, int item, int n, int search_k, int* result);
int ANNOY_API GetNItems(AnnoyIndexHandle handle);
int ANNOY_API GetNTrees(AnnoyIndexHandle handle);
void ANNOY_API SetVerbose(AnnoyIndexHandle handle, int verbose); // 0 or 1
void ANNOY_API GetItem(AnnoyIndexHandle handle, int item, float* out_vector);
void ANNOY_API SetSeed(AnnoyIndexHandle handle, int seed);
int ANNOY_API OnDiskBuild(AnnoyIndexHandle handle, const char* filename);


#ifdef __cplusplus
}
#endif

#endif //ANNOY_C_WRAPPER_H
