using System;
using System.Runtime.InteropServices;

namespace AnnoyWrapper
{
    public enum AnnoyMetric {
        Angular,
        Euclidean,
        Manhattan,
        DotProduct
    }

    public static class Annoy
    {
        #if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
            private const string Dll =  "annoy_c";
        #elif UNITY_EDITOR_OSX || UNITY_STANDALONE_OSX
            private const string Dll =  "libannoy_c";
        #elif UNITY_EDITOR_LINUX || UNITY_STANDALONE_LINUX || UNITY_ANDROID
            private const string Dll =  "libannoy_c";
        #else
            private const string Dll =  "annoy_c";
        #endif

        [DllImport(Dll)] public static extern IntPtr Index(int f, AnnoyMetric metric);
        [DllImport(Dll)] public static extern void AddItem(IntPtr handle, int item, float[] vector);
        [DllImport(Dll)] public static extern void Build(IntPtr handle, int nTrees);
        [DllImport(Dll)] public static extern void Save(IntPtr handle, string filename);
        [DllImport(Dll)] public static extern void Load(IntPtr handle, string filename);
        [DllImport(Dll)] public static extern void GetNnsByVector(IntPtr handle, float[] vector, int n, int[] result);
        [DllImport(Dll)] public static extern void GetNnsByItem(IntPtr handle, int item, int n, int search_k, int[] result);
        [DllImport(Dll)] public static extern float GetDistance(IntPtr handle, int i, int j);
        [DllImport(Dll)] public static extern int GetNItems(IntPtr handle);
        [DllImport(Dll)] public static extern int GetNTrees(IntPtr handle);
        [DllImport(Dll)] public static extern void SetVerbose(IntPtr handle, int verbose); // 0 or 1
        [DllImport(Dll)] public static extern void GetItem(IntPtr handle, int item, float[] out_vector);
        [DllImport(Dll)] public static extern void SetSeed(IntPtr handle, int seed);
        [DllImport(Dll)] public static extern int OnDiskBuild(IntPtr handle, string filename);
        [DllImport(Dll)] public static extern void Free(IntPtr handle);
    }
}