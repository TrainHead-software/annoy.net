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

namespace {
    typedef std::remove_const<decltype(Kiss32Random::default_seed)>::type FloatAnnoySeedType;
    typedef std::remove_const<decltype(Kiss64Random::default_seed)>::type HammingAnnoySeedType;

    class AnnoyIndexBridge {
    public:
        virtual ~AnnoyIndexBridge() = default;

        virtual bool AddItem(int item, const float* vector) = 0;
        virtual bool Build(int n_trees) = 0;
        virtual bool Save(const char* filename) = 0;
        virtual bool Load(const char* filename) = 0;
        virtual float GetDistance(int i, int j) const = 0;
        virtual void GetNnsByItem(int item, int n, int search_k, std::vector<int>* result) const = 0;
        virtual void GetNnsByVector(const float* vector, int n, std::vector<int>* result) const = 0;
        virtual int GetNItems() const = 0;
        virtual int GetNTrees() const = 0;
        virtual void SetVerbose(bool verbose) = 0;
        virtual void GetItem(int item, float* out_vector) const = 0;
        virtual void SetSeed(int seed) = 0;
        virtual bool OnDiskBuild(const char* filename) = 0;
    };

    template <typename IndexType>
    class FloatAnnoyIndexBridge final : public AnnoyIndexBridge {
    public:
        explicit FloatAnnoyIndexBridge(int f)
            : index_(f) {
        }

        bool AddItem(int item, const float* vector) override {
            return index_.add_item(item, vector);
        }

        bool Build(int n_trees) override {
            return index_.build(n_trees);
        }

        bool Save(const char* filename) override {
            return index_.save(filename);
        }

        bool Load(const char* filename) override {
            return index_.load(filename);
        }

        float GetDistance(int i, int j) const override {
            return index_.get_distance(i, j);
        }

        void GetNnsByItem(int item, int n, int search_k, std::vector<int>* result) const override {
            index_.get_nns_by_item(item, n, search_k, result, nullptr);
        }

        void GetNnsByVector(const float* vector, int n, std::vector<int>* result) const override {
            index_.get_nns_by_vector(vector, n, -1, result, nullptr);
        }

        int GetNItems() const override {
            return index_.get_n_items();
        }

        int GetNTrees() const override {
            return index_.get_n_trees();
        }

        void SetVerbose(bool verbose) override {
            index_.verbose(verbose);
        }

        void GetItem(int item, float* out_vector) const override {
            index_.get_item(item, out_vector);
        }

        void SetSeed(int seed) override {
            index_.set_seed(static_cast<FloatAnnoySeedType>(seed));
        }

        bool OnDiskBuild(const char* filename) override {
            return index_.on_disk_build(filename);
        }

    private:
        IndexType index_;
    };

    class HammingAnnoyIndexBridge final : public AnnoyIndexBridge {
    public:
        explicit HammingAnnoyIndexBridge(int f)
            : f_external_(f),
              f_internal_((f + 63) / 64),
              index_(f_internal_) {
        }

        bool AddItem(int item, const float* vector) override {
            std::vector<uint64_t> packed = Pack(vector);
            return index_.add_item(item, packed.data());
        }

        bool Build(int n_trees) override {
            return index_.build(n_trees);
        }

        bool Save(const char* filename) override {
            return index_.save(filename);
        }

        bool Load(const char* filename) override {
            return index_.load(filename);
        }

        float GetDistance(int i, int j) const override {
            return static_cast<float>(index_.get_distance(i, j));
        }

        void GetNnsByItem(int item, int n, int search_k, std::vector<int>* result) const override {
            index_.get_nns_by_item(item, n, search_k, result, nullptr);
        }

        void GetNnsByVector(const float* vector, int n, std::vector<int>* result) const override {
            std::vector<uint64_t> packed = Pack(vector);
            index_.get_nns_by_vector(packed.data(), n, -1, result, nullptr);
        }

        int GetNItems() const override {
            return index_.get_n_items();
        }

        int GetNTrees() const override {
            return index_.get_n_trees();
        }

        void SetVerbose(bool verbose) override {
            index_.verbose(verbose);
        }

        void GetItem(int item, float* out_vector) const override {
            std::vector<uint64_t> packed(f_internal_, 0);
            index_.get_item(item, packed.data());
            Unpack(packed.data(), out_vector);
        }

        void SetSeed(int seed) override {
            index_.set_seed(static_cast<HammingAnnoySeedType>(seed));
        }

        bool OnDiskBuild(const char* filename) override {
            return index_.on_disk_build(filename);
        }

    private:
        std::vector<uint64_t> Pack(const float* src) const {
            std::vector<uint64_t> packed(f_internal_, 0);
            for (int i = 0; i < f_internal_; ++i) {
                for (int bit = 0; bit < 64 && i * 64 + bit < f_external_; ++bit) {
                    if (src[i * 64 + bit] > 0.5f) {
                        packed[i] |= (uint64_t(1) << bit);
                    }
                }
            }

            return packed;
        }

        void Unpack(const uint64_t* src, float* dst) const {
            for (int i = 0; i < f_external_; ++i) {
                dst[i] = static_cast<float>((src[i / 64] >> (i % 64)) & 1ULL);
            }
        }

        int f_external_;
        int f_internal_;
        AnnoyIndex<int, uint64_t, Hamming, Kiss64Random, AnnoyIndexSingleThreadedBuildPolicy> index_;
    };

    struct AnnoyIndexHandleImpl {
        AnnoyMetric metric;
        AnnoyIndexBridge* index;
    };

    AnnoyIndexHandleImpl* UnwrapHandle(AnnoyIndexHandle handle) {
        return reinterpret_cast<AnnoyIndexHandleImpl*>(handle);
    }

    bool FillMissingResults(int n, int* result) {
        if (result == nullptr || n <= 0) {
            return false;
        }

        for (int i = 0; i < n; ++i) {
            result[i] = -1;
        }
        return true;
    }

    void CopyResults(const std::vector<int>& out, int n, int* result) {
        if (!FillMissingResults(n, result)) {
            return;
        }

        for (int i = 0; i < n; ++i) {
            result[i] = (i < static_cast<int>(out.size())) ? out[i] : -1;
        }
    }

    AnnoyIndexBridge* GetIndex(AnnoyIndexHandle handle) {
        AnnoyIndexHandleImpl* wrapped = UnwrapHandle(handle);
        if (wrapped == nullptr || wrapped->index == nullptr) {
            return nullptr;
        }

        return wrapped->index;
    }
}

extern "C" {
    AnnoyIndexHandle Index(int f, AnnoyMetric metric) {
        AnnoyIndexHandleImpl* wrapped = new AnnoyIndexHandleImpl{metric, nullptr};

        switch (metric) {
            case Metric_Angular:
                wrapped->index = new FloatAnnoyIndexBridge<AngularAnnoyIndexType>(f);
                break;
            case Metric_Euclidean:
                wrapped->index = new FloatAnnoyIndexBridge<EuclideanAnnoyIndexType>(f);
                break;
            case Metric_Manhattan:
                wrapped->index = new FloatAnnoyIndexBridge<ManhattanAnnoyIndexType>(f);
                break;
            case Metric_DotProduct:
                wrapped->index = new FloatAnnoyIndexBridge<DotProductAnnoyIndexType>(f);
                break;
            case Metric_Hamming:
                wrapped->index = new HammingAnnoyIndexBridge(f);
                break;
            default:
                delete wrapped;
                return nullptr;
        }

        return reinterpret_cast<AnnoyIndexHandle>(wrapped);
    }

    void AddItem(AnnoyIndexHandle handle, int item, float* vector) {
        AnnoyIndexBridge* index = GetIndex(handle);
        if (index == nullptr || vector == nullptr) {
            return;
        }

        index->AddItem(item, vector);
    }

    void Build(AnnoyIndexHandle handle, int n_trees) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            index->Build(n_trees);
        }
    }

    void Save(AnnoyIndexHandle handle, const char* filename) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            index->Save(filename);
        }
    }

    void Load(AnnoyIndexHandle handle, const char* filename) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            index->Load(filename);
        }
    }

    void GetNnsByVector(AnnoyIndexHandle handle, float* vector, int n, int* result) {
        AnnoyIndexBridge* index = GetIndex(handle);
        if (index == nullptr || vector == nullptr) {
            FillMissingResults(n, result);
            return;
        }

        std::vector<int> out;
        index->GetNnsByVector(vector, n, &out);
        CopyResults(out, n, result);
    }

    void Free(AnnoyIndexHandle handle) {
        AnnoyIndexHandleImpl* wrapped = UnwrapHandle(handle);
        if (wrapped == nullptr) {
            return;
        }

        delete wrapped->index;
        delete wrapped;
    }

    float GetDistance(AnnoyIndexHandle handle, int i, int j) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            return index->GetDistance(i, j);
        }

        return -1.0f;
    }

    void GetNnsByItem(AnnoyIndexHandle handle, int item, int n, int search_k, int* result) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            std::vector<int> out;
            index->GetNnsByItem(item, n, search_k, &out);
            CopyResults(out, n, result);
            return;
        }

        FillMissingResults(n, result);
    }

    int GetNItems(AnnoyIndexHandle handle) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            return index->GetNItems();
        }

        return -1;
    }

    int GetNTrees(AnnoyIndexHandle handle) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            return index->GetNTrees();
        }

        return -1;
    }

    void SetVerbose(AnnoyIndexHandle handle, int verbose) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            index->SetVerbose(verbose != 0);
        }
    }

    void GetItem(AnnoyIndexHandle handle, int item, float* out_vector) {
        AnnoyIndexBridge* index = GetIndex(handle);
        if (index == nullptr || out_vector == nullptr) {
            return;
        }

        index->GetItem(item, out_vector);
    }

    void SetSeed(AnnoyIndexHandle handle, int seed) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            index->SetSeed(seed);
        }
    }

    int OnDiskBuild(AnnoyIndexHandle handle, const char* filename) {
        if (AnnoyIndexBridge* index = GetIndex(handle)) {
            return index->OnDiskBuild(filename) ? 1 : 0;
        }

        return 0;
    }
}
