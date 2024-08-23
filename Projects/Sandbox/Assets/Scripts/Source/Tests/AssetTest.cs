using Odyssey;
using System;

namespace Sandbox
{
    public class AssetTest : Entity
    {
        public Mesh MeshAsset;

        protected override void Awake()
        {
            Console.WriteLine($"[AssetTest.Awake] Print Variables:" +
                $"\nMeshAsset: {MeshAsset != null}");
            Console.WriteLine($"[AssetTest.Awake] Setting MeshAsset.Name = AssetTest");

            MeshAsset.Name = "AssetTest";
            Console.WriteLine($"[AssetTest.Awake] Retrieving MeshAsset.Name = {MeshAsset.Name}");
        }
    }
}
