namespace Odyssey
{
    [NativeObject]
    public class Component
    {
        public Entity Entity { get; internal set; }

        internal Component() { }

        // Internal constructor so we can control how the object is created natively
        internal Component(ulong guid)
        {
            this.Entity = new Entity(guid);
        }
    }
}
