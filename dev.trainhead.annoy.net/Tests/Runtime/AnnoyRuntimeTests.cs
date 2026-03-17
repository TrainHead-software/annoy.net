using System;
using System.IO;
using NUnit.Framework;

namespace AnnoyWrapper.Tests
{
    public sealed class AnnoyRuntimeTests
    {
        [Test]
        public void Index_CapturesMetadataAndDisposesSafely()
        {
            AnnoyIndexHandle handle = Annoy.Index(3, AnnoyMetric.Angular);

            Assert.AreEqual(3, handle.Dimensions);
            Assert.AreEqual(AnnoyMetric.Angular, handle.Metric);

            Annoy.Free(handle);

            Assert.IsTrue(handle.IsClosed);
            Assert.Throws<ObjectDisposedException>(() => Annoy.GetNItems(handle));
        }

        [Test]
        public void AngularQuery_ReturnsExpectedNearestItem()
        {
            using (AnnoyIndexHandle index = Annoy.Index(3, AnnoyMetric.Angular))
            {
                Annoy.AddItem(index, 0, new[] { 1.0f, 0.0f, 0.0f });
                Annoy.AddItem(index, 1, new[] { 0.0f, 1.0f, 0.0f });
                Annoy.AddItem(index, 2, new[] { 0.0f, 0.0f, 1.0f });
                Annoy.Build(index, 10);

                int[] nearest = Annoy.GetNnsByVector(index, new[] { 1.0f, 0.2f, 0.2f }, 3);

                Assert.AreEqual(0, nearest[0]);
                Assert.AreEqual(3, Annoy.GetNItems(index));
            }
        }

        [Test]
        public void HammingQuery_RoundTripsBinaryVectors()
        {
            using (AnnoyIndexHandle index = Annoy.Index(4, AnnoyMetric.Hamming))
            {
                Annoy.AddItem(index, 0, new[] { 0.0f, 0.0f, 0.0f, 0.0f });
                Annoy.AddItem(index, 1, new[] { 1.0f, 0.0f, 0.0f, 0.0f });
                Annoy.AddItem(index, 2, new[] { 1.0f, 1.0f, 0.0f, 0.0f });
                Annoy.Build(index, 10);

                Assert.AreEqual(1.0f, Annoy.GetDistance(index, 0, 1), 0.0001f);
                Assert.AreEqual(2, Annoy.GetNnsByVector(index, new[] { 1.0f, 1.0f, 0.0f, 0.0f }, 3)[0]);
                CollectionAssert.AreEqual(new[] { 1.0f, 0.0f, 0.0f, 0.0f }, Annoy.GetItem(index, 1));
            }
        }

        [Test]
        public void OnDiskBuild_PersistsIndex()
        {
            string filename = Path.Combine(Path.GetTempPath(), "annoy-unity-" + Guid.NewGuid().ToString("N") + ".tree");

            try
            {
                using (AnnoyIndexHandle index = Annoy.Index(2, AnnoyMetric.Euclidean))
                {
                    Assert.AreEqual(1, Annoy.OnDiskBuild(index, filename));

                    Annoy.AddItem(index, 0, new[] { 2.0f, 2.0f });
                    Annoy.AddItem(index, 1, new[] { 3.0f, 2.0f });
                    Annoy.AddItem(index, 2, new[] { 3.0f, 3.0f });
                    Annoy.Build(index, 10);
                }

                using (AnnoyIndexHandle loaded = Annoy.Index(2, AnnoyMetric.Euclidean))
                {
                    Annoy.Load(loaded, filename);
                    Assert.AreEqual(2, Annoy.GetNnsByVector(loaded, new[] { 4.0f, 4.0f }, 3)[0]);
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
    }
}
