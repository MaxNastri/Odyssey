using Odyssey;
using System;

namespace Sandbox
{
    public class MeshRendererTest : Entity
    {
        public MeshRenderer MeshRenderer;
        public Mesh MeshTarget;

        protected override void Awake()
        {
            // Assign the mesh target
            MeshRenderer.Mesh = MeshTarget;

            Console.WriteLine($"[MeshRendererTest.Awake] Print Variables:" +
                $"\nMeshRenderer: {MeshRenderer != null}" +
                $"\nMeshRenderer.Mesh: {MeshRenderer.Mesh == MeshTarget}" +
                $"\nMeshRenderer.Mesh.Name: {MeshRenderer.Mesh.Name}" +
                $"\nMeshRenderer.Mesh.VertexCount: {MeshRenderer.Mesh.VertexCount}" +
                $"\nMeshRenderer.Mesh.IndexCount: {MeshRenderer.Mesh.IndexCount}");
        }
    }
}
