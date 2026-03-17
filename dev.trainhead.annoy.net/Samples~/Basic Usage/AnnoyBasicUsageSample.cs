using UnityEngine;

namespace AnnoyWrapper.Samples
{
    public sealed class AnnoyBasicUsageSample : MonoBehaviour
    {
        private void Start()
        {
            using (AnnoyIndexHandle index = Annoy.Index(3, AnnoyMetric.Angular))
            {
                Annoy.AddItem(index, 0, new[] { 1.0f, 0.0f, 0.0f });
                Annoy.AddItem(index, 1, new[] { 0.0f, 1.0f, 0.0f });
                Annoy.AddItem(index, 2, new[] { 0.0f, 0.0f, 1.0f });
                Annoy.Build(index, 10);

                int[] nearest = Annoy.GetNnsByVector(index, new[] { 1.0f, 0.2f, 0.2f }, 3);
                Debug.Log("Nearest Annoy item: " + nearest[0]);
            }
        }
    }
}
