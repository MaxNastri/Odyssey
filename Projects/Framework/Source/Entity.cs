using Coral.Managed.Interop;
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
            GUID = new GUID(0);
        }

        // Internal constructor so we can control how the object is created natively
        internal Entity(ulong guid)
        {
            GUID = new GUID(guid);
        }

        internal Entity(GUID guid)
        {
            GUID = guid;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);

            // Check if we already have a component of this type
            if (HasComponent<T>())
                return GetComponent<T>();

            // Add the internal component
            unsafe { InternalCalls.GameObject_AddComponent(GUID, type); }

            // Construct a new component and cache it
            T component = new T() { Entity = this };
            componentCache[type] = component;

            return component;
        }

        public bool HasComponent<T>() where T : Component
        {
            unsafe { return InternalCalls.GameObject_HasComponent(GUID, typeof(T)); }
        }

        public bool RemoveComponent<T>() where T : Component
        {
            unsafe { return InternalCalls.GameObject_RemoveComponent(GUID, typeof(T)); }
        }

        public T GetScript<T>() where T : Component, new()
        {
            if (HasComponent<ScriptComponent>())
            {
                unsafe
                {
                    NativeInstance<object> instance = InternalCalls.GameObject_GetScript(this.GUID);
                    if (instance.Get() is T scriptInstance)
                        return scriptInstance;
                }
            }

            return null;
        }

        public T GetComponent<T>() where T : Component, new()
        {
            Type type = typeof(T);

            // Check the game object for this type
            if (!HasComponent<T>())
            {
                // The game object doesn't have this type, remove any instances from the cache
                componentCache.Remove(type);
                return null;
            }

            // The native game object has the component, but its not in the cache
            if (!componentCache.ContainsKey(type))
            {
                // Construct a new managed component and cache it
                T component = new T() { Entity = this };
                componentCache[type] = component;
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

        public void Destroy()
        {
            unsafe { InternalCalls.GameObject_Destroy(GUID); }
        }

        public string Name
        {
            get { unsafe { return InternalCalls.GameObject_GetName(GUID); } }
            set { unsafe { InternalCalls.GameObject_SetName(GUID, value); } }
        }
    }
}
