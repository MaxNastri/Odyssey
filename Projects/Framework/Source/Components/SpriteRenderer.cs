using System.Text.RegularExpressions;

namespace Odyssey
{
    public class SpriteRenderer : Component
    {
        public Vector2 Fill
        {
            get
            {
                unsafe
                {
                    Vector2 value = new Vector2();
                    InternalCalls.SpriteRenderer_GetFill(Entity.GUID, &value);
                    return value;
                }
            }
            set
            {
                unsafe { InternalCalls.SpriteRenderer_SetFill(Entity.GUID, value); }
            }
        }

        public Color BaseColor
        {
            get
            {
                unsafe
                {
                    Color color = new Color();
                    InternalCalls.SpriteRenderer_GetBaseColor(Entity.GUID, &color);
                    return color;
                }
            }
            set
            {
                unsafe {  InternalCalls.SpriteRenderer_SetBaseColor(Entity.GUID, value); }
            }
        }

        public Texture2D Sprite
        {
            get
            {
                unsafe
                {
                    GUID spriteGUID;
                    InternalCalls.SpriteRenderer_GetSprite(Entity.GUID, &spriteGUID);
                    return new Texture2D(spriteGUID);
                }
            }
            set
            {
                unsafe
                {
                    InternalCalls.SpriteRenderer_SetSprite(Entity.GUID, value.Guid);
                }
            }
        }
    }
}
