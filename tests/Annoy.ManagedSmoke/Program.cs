using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using AnnoyWrapper;

internal static class Program
{
    private static int Main()
    {
        NativeLibrary.SetDllImportResolver(typeof(Annoy).Assembly, ResolveNativeLibrary);

        TestHandleMetadataAndDisposal();
        TestAngularFlow();
        TestHammingFlow();
        TestOnDiskBuild();
        TestValidation();

        Console.WriteLine("Managed smoke tests passed.");
        return 0;
    }

    private static void TestHandleMetadataAndDisposal()
    {
        AnnoyIndexHandle handle = Annoy.Index(3, AnnoyMetric.Angular);
        Assert(handle.Dimensions == 3, "Handle dimensions should match the created index.");
        Assert(handle.Metric == AnnoyMetric.Angular, "Handle metric should match the created index.");

        Annoy.Free(handle);

        Assert(handle.IsClosed, "Handle should be closed after Annoy.Free.");
        AssertThrows<ObjectDisposedException>(() => Annoy.GetNItems(handle), "Disposed handles must throw on access.");
    }

    private static void TestAngularFlow()
    {
        using (AnnoyIndexHandle index = Annoy.Index(3, AnnoyMetric.Angular))
        {
            Annoy.SetVerbose(index, true);
            Annoy.SetVerbose(index, false);
            Annoy.SetSeed(index, 42);

            Annoy.AddItem(index, 0, new[] { 1.0f, 0.0f, 0.0f });
            Annoy.AddItem(index, 1, new[] { 0.0f, 1.0f, 0.0f });
            Annoy.AddItem(index, 2, new[] { 0.0f, 0.0f, 1.0f });
            Annoy.Build(index, 10);

            Assert(Annoy.GetNItems(index) == 3, "Angular index should report all added items.");
            Assert(Annoy.GetNTrees(index) > 0, "Angular index should report built trees.");

            int[] byVector = Annoy.GetNnsByVector(index, new[] { 1.0f, 0.2f, 0.2f }, 3);
            Assert(byVector[0] == 0, "Angular nearest-neighbor query should return item 0 first.");

            int[] byItem = Annoy.GetNnsByItem(index, 1, 3);
            Assert(byItem[0] == 1, "Angular item query should return the queried item first.");

            float[] item = Annoy.GetItem(index, 1);
            AssertFloatNear(item[0], 0.0f, 0.0001f, "Angular item vector should be readable.");
            AssertFloatNear(item[1], 1.0f, 0.0001f, "Angular item vector should be readable.");
            AssertFloatNear(item[2], 0.0f, 0.0001f, "Angular item vector should be readable.");

            AssertFloatNear(Annoy.GetDistance(index, 0, 1), 1.4142135f, 0.0001f, "Angular distance should match expected value.");
        }
    }

    private static void TestHammingFlow()
    {
        using (AnnoyIndexHandle index = Annoy.Index(4, AnnoyMetric.Hamming))
        {
            Annoy.AddItem(index, 0, new[] { 0.0f, 0.0f, 0.0f, 0.0f });
            Annoy.AddItem(index, 1, new[] { 1.0f, 0.0f, 0.0f, 0.0f });
            Annoy.AddItem(index, 2, new[] { 1.0f, 1.0f, 0.0f, 0.0f });
            Annoy.Build(index, 10);

            AssertFloatNear(Annoy.GetDistance(index, 0, 1), 1.0f, 0.0001f, "Hamming distance between binary vectors should be 1.");
            AssertFloatNear(Annoy.GetDistance(index, 0, 2), 2.0f, 0.0001f, "Hamming distance between binary vectors should be 2.");

            int[] byVector = Annoy.GetNnsByVector(index, new[] { 1.0f, 1.0f, 0.0f, 0.0f }, 3);
            Assert(byVector[0] == 2, "Hamming nearest-neighbor query should return the exact binary match.");

            float[] item = Annoy.GetItem(index, 1);
            AssertFloatNear(item[0], 1.0f, 0.0001f, "Hamming item vector should round-trip through the managed API.");
            AssertFloatNear(item[1], 0.0f, 0.0001f, "Hamming item vector should round-trip through the managed API.");
            AssertFloatNear(item[2], 0.0f, 0.0001f, "Hamming item vector should round-trip through the managed API.");
            AssertFloatNear(item[3], 0.0f, 0.0001f, "Hamming item vector should round-trip through the managed API.");
        }
    }

    private static void TestOnDiskBuild()
    {
        string filename = Path.Combine(Path.GetTempPath(), "annoy-managed-" + Guid.NewGuid().ToString("N") + ".tree");

        try
        {
            using (AnnoyIndexHandle index = Annoy.Index(2, AnnoyMetric.Euclidean))
            {
                Assert(Annoy.OnDiskBuild(index, filename) == 1, "OnDiskBuild should return success.");
                Annoy.AddItem(index, 0, new[] { 2.0f, 2.0f });
                Annoy.AddItem(index, 1, new[] { 3.0f, 2.0f });
                Annoy.AddItem(index, 2, new[] { 3.0f, 3.0f });
                Annoy.Build(index, 10);

                int[] query = Annoy.GetNnsByVector(index, new[] { 4.0f, 4.0f }, 3);
                Assert(query[0] == 2, "On-disk index should answer vector queries after build.");
            }

            using (AnnoyIndexHandle loaded = Annoy.Index(2, AnnoyMetric.Euclidean))
            {
                Annoy.Load(loaded, filename);
                int[] query = Annoy.GetNnsByVector(loaded, new[] { 4.0f, 4.0f }, 3);
                Assert(query[0] == 2, "Loaded on-disk index should answer vector queries.");
            }
        }
        finally
        {
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
        }
    }

    private static void TestValidation()
    {
        AssertThrows<ArgumentOutOfRangeException>(() => Annoy.Index(0, AnnoyMetric.Angular), "Index dimensions should be validated.");

        using (AnnoyIndexHandle index = Annoy.Index(3, AnnoyMetric.Angular))
        {
            AssertThrows<ArgumentException>(() => Annoy.AddItem(index, 0, new float[2]), "AddItem should reject vectors with invalid length.");
            AssertThrows<ArgumentException>(() => Annoy.GetItem(index, 0, new float[2]), "GetItem should reject buffers with invalid length.");
            AssertThrows<ArgumentOutOfRangeException>(() => Annoy.GetNnsByVector(index, new float[3], -1), "Neighbor count should be validated.");
            AssertThrows<ArgumentException>(() => Annoy.Save(index, string.Empty), "Save should reject empty filenames.");
        }
    }

    private static IntPtr ResolveNativeLibrary(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        if (libraryName != "annoy_c" && libraryName != "libannoy_c")
        {
            return IntPtr.Zero;
        }

        return NativeLibrary.Load(ResolveNativeLibraryPath());
    }

    private static string ResolveNativeLibraryPath()
    {
        string envPath = Environment.GetEnvironmentVariable("ANNOY_NATIVE_LIBRARY_PATH");
        if (!string.IsNullOrWhiteSpace(envPath) && File.Exists(envPath))
        {
            return Path.GetFullPath(envPath);
        }

        string repoRoot = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", ".."));
        string fileName = GetNativeFileName();
        string[] candidates =
        {
            Path.Combine(repoRoot, "build", "Debug", fileName),
            Path.Combine(repoRoot, "build", "Release", fileName),
            Path.Combine(repoRoot, "annoy_clang", "build", "Debug", fileName),
            Path.Combine(repoRoot, "annoy_clang", "build", "Release", fileName)
        };

        foreach (string candidate in candidates)
        {
            if (File.Exists(candidate))
            {
                return candidate;
            }
        }

        throw new FileNotFoundException(
            "Native Annoy library was not found. Set ANNOY_NATIVE_LIBRARY_PATH to the built library path.");
    }

    private static string GetNativeFileName()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            return "annoy_c.dll";
        }

        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            return "libannoy_c.dylib";
        }

        return "libannoy_c.so";
    }

    private static void Assert(bool condition, string message)
    {
        if (!condition)
        {
            throw new InvalidOperationException(message);
        }
    }

    private static void AssertFloatNear(float actual, float expected, float tolerance, string message)
    {
        if (Math.Abs(actual - expected) > tolerance)
        {
            throw new InvalidOperationException(message + " Expected " + expected + " but got " + actual + ".");
        }
    }

    private static void AssertThrows<TException>(Action action, string message) where TException : Exception
    {
        try
        {
            action();
        }
        catch (TException)
        {
            return;
        }

        throw new InvalidOperationException(message + " Expected exception " + typeof(TException).Name + ".");
    }
}
