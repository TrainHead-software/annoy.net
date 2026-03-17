using System;
using System.IO;
using AnnoyWrapper;

internal static class Program
{
    private static int Main()
    {
        TestAngularFlow();
        TestHammingFlow();
        TestOnDiskBuild();
        TestValidation();

        Console.WriteLine("NuGet smoke tests passed.");
        return 0;
    }

    private static void TestAngularFlow()
    {
        using (AnnoyIndexHandle index = Annoy.Index(3, AnnoyMetric.Angular))
        {
            Annoy.AddItem(index, 0, new[] { 1.0f, 0.0f, 0.0f });
            Annoy.AddItem(index, 1, new[] { 0.0f, 1.0f, 0.0f });
            Annoy.AddItem(index, 2, new[] { 0.0f, 0.0f, 1.0f });
            Annoy.Build(index, 10);

            Assert(Annoy.GetNItems(index) == 3, "Angular NuGet flow should report all items.");
            Assert(Annoy.GetNnsByVector(index, new[] { 1.0f, 0.2f, 0.2f }, 3)[0] == 0, "Angular NuGet flow should load the native library.");
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

            AssertFloatNear(Annoy.GetDistance(index, 0, 1), 1.0f, 0.0001f, "Hamming NuGet flow should work through the packaged native library.");
            Assert(Annoy.GetNnsByVector(index, new[] { 1.0f, 1.0f, 0.0f, 0.0f }, 3)[0] == 2, "Hamming NuGet flow should return the exact match.");
        }
    }

    private static void TestOnDiskBuild()
    {
        string filename = Path.Combine(Path.GetTempPath(), "annoy-nuget-" + Guid.NewGuid().ToString("N") + ".tree");

        try
        {
            using (AnnoyIndexHandle index = Annoy.Index(2, AnnoyMetric.Euclidean))
            {
                Assert(Annoy.OnDiskBuild(index, filename) == 1, "NuGet OnDiskBuild should succeed.");
                Annoy.AddItem(index, 0, new[] { 2.0f, 2.0f });
                Annoy.AddItem(index, 1, new[] { 3.0f, 2.0f });
                Annoy.AddItem(index, 2, new[] { 3.0f, 3.0f });
                Annoy.Build(index, 10);
            }

            using (AnnoyIndexHandle loaded = Annoy.Index(2, AnnoyMetric.Euclidean))
            {
                Annoy.Load(loaded, filename);
                Assert(Annoy.GetNnsByVector(loaded, new[] { 4.0f, 4.0f }, 3)[0] == 2, "NuGet-loaded index should answer queries.");
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
        AssertThrows<ArgumentOutOfRangeException>(() => Annoy.Index(0, AnnoyMetric.Angular), "NuGet API should preserve managed validation.");
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
