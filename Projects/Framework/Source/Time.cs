namespace Odyssey
{
    public static class Time
    {
        public static float DeltaTime
        {
            get { unsafe { return InternalCalls.Time_GetDeltaTime(); } }
        }
    }
}
