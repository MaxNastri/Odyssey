using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Odyssey.Managed.Core
{
    public class Component
    {
        public virtual void Awake() { }
        public virtual void Update() { }
        public virtual void OnDestroy() { }
    }

}
