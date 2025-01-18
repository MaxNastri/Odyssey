namespace Odyssey
{
    public enum EmitterShape
    {
        Circle = 0,
        Cone = 1,
        Cube = 2,
        Donut = 3,
        Sphere = 4,
        Mesh = 5,
    }

    public class ParticleEmitter : Component
    {
        public bool Looping
        {
            get
            {
                unsafe
                {
                    bool looping = false;
                    InternalCalls.ParticleEmitter_GetLooping(Entity.GUID, &looping);
                    return looping;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetLooping(Entity.GUID, value);
                }
            }
        }


        public uint EmissionRate
        {
            get
            {
                unsafe
                {
                    uint value = 0;
                    InternalCalls.ParticleEmitter_GetEmissionRate(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetEmissionRate(Entity.GUID, value);
                }
            }
        }


        public float Radius
        {
            get
            {
                unsafe
                {
                    float value = 0.0f;
                    InternalCalls.ParticleEmitter_GetRadius(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetRadius(Entity.GUID, value);
                }
            }
        }


        public float Angle
        {
            get
            {
                unsafe
                {
                    float value = 0.0f;
                    InternalCalls.ParticleEmitter_GetAngle(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetAngle(Entity.GUID, value);
                }
            }
        }


        public float Duration
        {
            get
            {
                unsafe
                {
                    float value = 0.0f;
                    InternalCalls.ParticleEmitter_GetDuration(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetDuration(Entity.GUID, value);
                }
            }
        }

        public Vector2 Lifetime
        {
            get
            {
                unsafe
                {
                    Vector2 value;
                    InternalCalls.ParticleEmitter_GetLifetime(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetLifetime(Entity.GUID, value);
                }
            }
        }

        public Vector2 Size
        {
            get
            {
                unsafe
                {
                    Vector2 value;
                    InternalCalls.ParticleEmitter_GetSize(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetSize(Entity.GUID, value);
                }
            }
        }

        public Vector2 Speed
        {
            get
            {
                unsafe
                {
                    Vector2 value;
                    InternalCalls.ParticleEmitter_GetSpeed(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetSpeed(Entity.GUID, value);
                }
            }
        }

        public Color StartColor
        {
            get
            {
                unsafe
                {
                    Color value;
                    InternalCalls.ParticleEmitter_GetStartColor(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetStartColor(Entity.GUID, value);
                }
            }
        }

        public Color EndColor
        {
            get
            {
                unsafe
                {
                    Color value;
                    InternalCalls.ParticleEmitter_GetEndColor(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetEndColor(Entity.GUID, value);
                }
            }
        }

        public EmitterShape Shape
        {
            get
            {
                unsafe
                {
                    EmitterShape value;
                    InternalCalls.ParticleEmitter_GetShape(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.ParticleEmitter_SetShape(Entity.GUID, value);
                }
            }
        }
    }
}
