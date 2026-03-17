using System;
using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;

namespace AnnoyWrapper
{
    public enum AnnoyMetric
    {
        Angular = 0,
        Euclidean = 1,
        Manhattan = 2,
        DotProduct = 3,
        Hamming = 4
    }

    public sealed class AnnoyIndexHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        internal AnnoyIndexHandle()
            : base(true)
        {
        }

        internal AnnoyIndexHandle(IntPtr existingHandle, int dimensions, AnnoyMetric metric)
            : base(true)
        {
            if (existingHandle == IntPtr.Zero)
            {
                throw new ArgumentException("Native index handle must not be zero.", nameof(existingHandle));
            }

            SetHandle(existingHandle);
            Dimensions = dimensions;
            Metric = metric;
        }

        public int Dimensions { get; private set; }

        public AnnoyMetric Metric { get; private set; }

        protected override bool ReleaseHandle()
        {
            AnnoyNative.Free(handle);
            return true;
        }
    }

    public static class Annoy
    {
        public static AnnoyIndexHandle Index(int dimensions, AnnoyMetric metric)
        {
            if (dimensions <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(dimensions), "Dimensions must be greater than zero.");
            }

            IntPtr nativeHandle = AnnoyNative.Index(dimensions, metric);
            if (nativeHandle == IntPtr.Zero)
            {
                throw new InvalidOperationException("Failed to create Annoy index.");
            }

            return new AnnoyIndexHandle(nativeHandle, dimensions, metric);
        }

        public static void AddItem(AnnoyIndexHandle handle, int item, float[] vector)
        {
            ValidateItem(item);
            ValidateVector(handle, vector, nameof(vector));
            AnnoyNative.AddItem(handle, item, vector);
        }

        public static void Build(AnnoyIndexHandle handle, int nTrees)
        {
            ValidateHandle(handle);
            if (nTrees < -1)
            {
                throw new ArgumentOutOfRangeException(nameof(nTrees), "Tree count must be greater than or equal to -1.");
            }

            AnnoyNative.Build(handle, nTrees);
        }

        public static void Save(AnnoyIndexHandle handle, string filename)
        {
            ValidateHandle(handle);
            ValidateFilename(filename, nameof(filename));
            AnnoyNative.Save(handle, filename);
        }

        public static void Load(AnnoyIndexHandle handle, string filename)
        {
            ValidateHandle(handle);
            ValidateFilename(filename, nameof(filename));
            AnnoyNative.Load(handle, filename);
        }

        public static int[] GetNnsByVector(AnnoyIndexHandle handle, float[] vector, int n)
        {
            int[] result = new int[ValidateNeighborCount(n)];
            GetNnsByVector(handle, vector, n, result);
            return result;
        }

        public static void GetNnsByVector(AnnoyIndexHandle handle, float[] vector, int n, int[] result)
        {
            ValidateVector(handle, vector, nameof(vector));
            ValidateResultBuffer(n, result, nameof(result));
            AnnoyNative.GetNnsByVector(handle, vector, n, result);
        }

        public static int[] GetNnsByItem(AnnoyIndexHandle handle, int item, int n, int searchK = -1)
        {
            int[] result = new int[ValidateNeighborCount(n)];
            GetNnsByItem(handle, item, n, searchK, result);
            return result;
        }

        public static void GetNnsByItem(AnnoyIndexHandle handle, int item, int n, int searchK, int[] result)
        {
            ValidateHandle(handle);
            ValidateItem(item);
            ValidateResultBuffer(n, result, nameof(result));
            if (searchK < -1)
            {
                throw new ArgumentOutOfRangeException(nameof(searchK), "searchK must be greater than or equal to -1.");
            }

            AnnoyNative.GetNnsByItem(handle, item, n, searchK, result);
        }

        public static float GetDistance(AnnoyIndexHandle handle, int i, int j)
        {
            ValidateHandle(handle);
            ValidateItem(i);
            ValidateItem(j);
            return AnnoyNative.GetDistance(handle, i, j);
        }

        public static int GetNItems(AnnoyIndexHandle handle)
        {
            ValidateHandle(handle);
            return AnnoyNative.GetNItems(handle);
        }

        public static int GetNTrees(AnnoyIndexHandle handle)
        {
            ValidateHandle(handle);
            return AnnoyNative.GetNTrees(handle);
        }

        public static void SetVerbose(AnnoyIndexHandle handle, bool verbose)
        {
            SetVerbose(handle, verbose ? 1 : 0);
        }

        public static void SetVerbose(AnnoyIndexHandle handle, int verbose)
        {
            ValidateHandle(handle);
            if (verbose != 0 && verbose != 1)
            {
                throw new ArgumentOutOfRangeException(nameof(verbose), "Verbose flag must be either 0 or 1.");
            }

            AnnoyNative.SetVerbose(handle, verbose);
        }

        public static float[] GetItem(AnnoyIndexHandle handle, int item)
        {
            ValidateHandle(handle);
            float[] vector = new float[handle.Dimensions];
            GetItem(handle, item, vector);
            return vector;
        }

        public static void GetItem(AnnoyIndexHandle handle, int item, float[] outVector)
        {
            ValidateItem(item);
            ValidateVector(handle, outVector, nameof(outVector));
            AnnoyNative.GetItem(handle, item, outVector);
        }

        public static void SetSeed(AnnoyIndexHandle handle, int seed)
        {
            ValidateHandle(handle);
            AnnoyNative.SetSeed(handle, seed);
        }

        public static int OnDiskBuild(AnnoyIndexHandle handle, string filename)
        {
            ValidateHandle(handle);
            ValidateFilename(filename, nameof(filename));
            return AnnoyNative.OnDiskBuild(handle, filename);
        }

        public static bool TryOnDiskBuild(AnnoyIndexHandle handle, string filename)
        {
            return OnDiskBuild(handle, filename) != 0;
        }

        public static void Free(AnnoyIndexHandle handle)
        {
            if (handle == null)
            {
                return;
            }

            handle.Dispose();
        }

        private static void ValidateHandle(AnnoyIndexHandle handle)
        {
            if (handle == null)
            {
                throw new ArgumentNullException(nameof(handle));
            }

            if (handle.IsClosed || handle.IsInvalid)
            {
                throw new ObjectDisposedException(nameof(handle));
            }
        }

        private static void ValidateItem(int item)
        {
            if (item < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(item), "Item index must be greater than or equal to zero.");
            }
        }

        private static void ValidateVector(AnnoyIndexHandle handle, float[] vector, string paramName)
        {
            ValidateHandle(handle);
            if (vector == null)
            {
                throw new ArgumentNullException(paramName);
            }

            if (vector.Length != handle.Dimensions)
            {
                throw new ArgumentException(
                    string.Format("Vector length must match the index dimensions ({0}).", handle.Dimensions),
                    paramName);
            }
        }

        private static int ValidateNeighborCount(int n)
        {
            if (n < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(n), "Neighbor count must be greater than or equal to zero.");
            }

            return n;
        }

        private static void ValidateResultBuffer(int n, int[] result, string paramName)
        {
            ValidateNeighborCount(n);
            if (result == null)
            {
                throw new ArgumentNullException(paramName);
            }

            if (result.Length < n)
            {
                throw new ArgumentException("Result buffer length must be greater than or equal to n.", paramName);
            }
        }

        private static void ValidateFilename(string filename, string paramName)
        {
            if (string.IsNullOrWhiteSpace(filename))
            {
                throw new ArgumentException("Filename must not be null or empty.", paramName);
            }
        }
    }

    internal static class AnnoyNative
    {
        private const string Dll = "annoy_c";

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern IntPtr Index(int f, AnnoyMetric metric);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void AddItem(AnnoyIndexHandle handle, int item, float[] vector);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Build(AnnoyIndexHandle handle, int nTrees);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void Save(AnnoyIndexHandle handle, string filename);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void Load(AnnoyIndexHandle handle, string filename);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void GetNnsByVector(AnnoyIndexHandle handle, float[] vector, int n, int[] result);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void GetNnsByItem(AnnoyIndexHandle handle, int item, int n, int searchK, int[] result);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern float GetDistance(AnnoyIndexHandle handle, int i, int j);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int GetNItems(AnnoyIndexHandle handle);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int GetNTrees(AnnoyIndexHandle handle);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void SetVerbose(AnnoyIndexHandle handle, int verbose);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void GetItem(AnnoyIndexHandle handle, int item, float[] outVector);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void SetSeed(AnnoyIndexHandle handle, int seed);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern int OnDiskBuild(AnnoyIndexHandle handle, string filename);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Free(IntPtr handle);
    }
}
