//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "annoy_c_wrapper.h"
#include "annoylib.h"
#include "kissrandom.h"

using namespace std;
using namespace Annoy;

typedef AnnoyIndex<int, float, Angular, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> AngularAnnoyIndexType;
typedef AnnoyIndex<int, float, Euclidean, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> EuclideanAnnoyIndexType;
typedef AnnoyIndex<int, float, Manhattan, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> ManhattanAnnoyIndexType;
typedef AnnoyIndex<int, float, DotProduct, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> DotProductAnnoyIndexType;
//typedef AnnoyIndex<int, float, Hamming, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> HammingAnnoyIndexType;

extern "C" {

    AnnoyIndexHandle Index(int f, AnnoyMetric metric) {
        switch (metric) {
            case Metric_Angular:   return new AngularAnnoyIndexType(f);
            case Metric_Euclidean: return new EuclideanAnnoyIndexType(f);
            case Metric_Manhattan: return new ManhattanAnnoyIndexType(f);
            case Metric_DotProduct:return new DotProductAnnoyIndexType(f);
            default: return nullptr;
        }
    }

    void AddItem(AnnoyIndexHandle handle, int item, float* vector) {
        ((AnnoyIndexInterface<int, float>*)handle)->add_item(item, vector);
    }

    void Build(AnnoyIndexHandle handle, int n_trees) {
        ((AnnoyIndexInterface<int, float>*)handle)->build(n_trees);
    }

    void Save(AnnoyIndexHandle handle, const char* filename) {
        ((AnnoyIndexInterface<int, float>*)handle)->save(filename);
    }

    void Load(AnnoyIndexHandle handle, const char* filename) {
        ((AnnoyIndexInterface<int, float>*)handle)->load(filename);
    }

    void GetNnsByVector(AnnoyIndexHandle handle, float* vector, int n, int* result) {
        std::vector<int> out;
        ((AnnoyIndexInterface<int, float>*)handle)->get_nns_by_vector(vector, n, -1, &out, nullptr);
        for (int i = 0; i < n; ++i) {
            result[i] = (i < out.size()) ? out[i] : -1;
        }
    }

    void Free(AnnoyIndexHandle handle) {
        delete (AnnoyIndexInterface<int, float>*)handle;
    }

    float GetDistance(AnnoyIndexHandle handle, int i, int j) {
        return ((AnnoyIndexInterface<int, float>*)handle)->get_distance(i, j);
    }

    void GetNnsByItem(AnnoyIndexHandle handle, int item, int n, int search_k, int* result) {
        std::vector<int> out;
        ((AnnoyIndexInterface<int, float>*)handle)->get_nns_by_item(item, n, search_k, &out, nullptr);
        for (int i = 0; i < n; ++i)
            result[i] = (i < out.size()) ? out[i] : -1;
    }

    int GetNItems(AnnoyIndexHandle handle) {
        return ((AnnoyIndexInterface<int, float>*)handle)->get_n_items();
    }

    int GetNTrees(AnnoyIndexHandle handle) {
        return ((AnnoyIndexInterface<int, float>*)handle)->get_n_trees();
    }

    void SetVerbose(AnnoyIndexHandle handle, int verbose) {
        ((AnnoyIndexInterface<int, float>*)handle)->verbose(verbose != 0);
    }

    void GetItem(AnnoyIndexHandle handle, int item, float* out_vector) {
        ((AnnoyIndexInterface<int, float>*)handle)->get_item(item, out_vector);
    }

    void SetSeed(AnnoyIndexHandle handle, int seed) {
        ((AnnoyIndexInterface<int, float>*)handle)->set_seed(seed);
    }

    int OnDiskBuild(AnnoyIndexHandle handle, const char* filename) {
        bool success = ((AnnoyIndexInterface<int, float>*)handle)->on_disk_build(filename);
        return success ? 1 : 0;
    }
}
