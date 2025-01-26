using System;

namespace Odyssey
{
    [NativeObject]
    public abstract class Component
    {
        public Entity Entity { get; internal set; }

        public Component() { }

        // Internal constructor so we can control how the object is created natively
        internal Component(ulong guid)
        {
            Entity = new Entity(guid);
        }

        protected virtual void Awake() { }
        protected virtual void Update() { }
        protected virtual void OnDestroy() { }
        protected virtual void OnCollisionEnter(Entity entity, Vector3 contactNormal) { }
        protected virtual void OnCollisionStay(Entity entity, Vector3 contactNormal) { }
        protected virtual void OnCollisionExit(Entity entity) { }

        public T AddComponent<T>() where T : Component, new()
        {
            return Entity.AddComponent<T>();
        }

        public bool HasComponent<T>() where T : Component
        {
            return Entity.HasComponent<T>();
        }

        public bool RemoveComponent<T>() where T : Component
        {
            return Entity.RemoveComponent<T>();
        }

        public T GetComponent<T>() where T : Component, new()
        {
            return Entity.GetComponent<T>();
        }

        private void OnCollisionEnterInternal(ulong guid, Vector3 contactNormal) => OnCollisionEnter(new Entity(guid), contactNormal);
        private void OnCollisionStayInternal(ulong guid, Vector3 contactNormal) => OnCollisionStay(new Entity(guid), contactNormal);
        private void OnCollisionExitInternal(ulong guid) => OnCollisionExit(new Entity(guid));
    }
}
