using System;
using System.Collections.Generic;

namespace Odyssey
{
    [NativeObject]
    public class Entity
    {
        private Dictionary<Type, Component> componentCache = new Dictionary<Type, Component>();
        
        internal GUID GUID { get; set; }

        protected Entity()
        {
            this.GUID = new GUID(0);
        }

        // Internal constructor so we can control how the object is created natively
        internal Entity(ulong guid)
        {
            this.GUID = new GUID(guid);
        }

        internal Entity(GUID guid)
        {
            this.GUID = guid;
        }

        protected virtual void Awake() { }
        protected virtual void Update() { }
        protected virtual void OnDestroy() { }

        public T AddComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);
            
            // Check if we already have a component of this type
            if (this.HasComponent<T>())
                return this.GetComponent<T>();

            // Add the internal component
            unsafe { InternalCalls.GameObject_AddComponent(this.GUID, type); }

            // Construct a new component and cache it
            T component = new T() { Entity = this };
            this.componentCache[type] = component;

            return component;
        }

        public bool HasComponent<T>() where T : Component
        {
            unsafe { return InternalCalls.GameObject_HasComponent(this.GUID, typeof(T)); }
        }

        public bool RemoveComponent<T>() where T : Component
        {
            unsafe { return InternalCalls.GameObject_RemoveComponent(this.GUID, typeof(T)); }
        }

        public T GetComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);

            // Check the game object for this type
            if (!this.HasComponent<T>())
            {
                // The game object doesn't have this type, remove any instances from the cache
                this.componentCache.Remove(type);
                return null;
            }

            // The native game object has the component, but its not in the cache
            if (!this.componentCache.ContainsKey(type))
            {
                // Construct a new managed component and cache it
                T component = new T() { Entity = this };
                this.componentCache[type] = component;
                return component;
            }

            return this.componentCache[type] as T;
        }

        public static Entity Create()
        {
            GUID guid;
            unsafe { guid = InternalCalls.GameObject_Create(); }
            return new Entity(guid);
        }

        public string Name
        {
            get { unsafe { return InternalCalls.GameObject_GetName(this.GUID); } }
            set { unsafe { InternalCalls.GameObject_SetName(this.GUID, value); } }
        }
    }
}
