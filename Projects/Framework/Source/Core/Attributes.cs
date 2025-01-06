using System;

namespace Odyssey
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = true)]
    internal class NativeObjectAttribute : Attribute { }
}
