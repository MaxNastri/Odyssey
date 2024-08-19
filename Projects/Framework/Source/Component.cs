using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Odyssey
{
    public class Component
    {
        internal Component() { }

        // Internal constructor so we can control how the object is created natively
        internal Component(ulong guid)
        {
            this.Entity = new Entity(guid);
        }

        public Entity Entity { get; internal set; }
    }
}
