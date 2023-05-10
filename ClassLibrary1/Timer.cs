using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SY
{
    public class Timer
    {
        public static void SetTimeScale(float timeScale)
        {
            InternalCalls.Timer_SetTimeScale(timeScale);
        }
        public static float GetTimeScale()
        {
            return InternalCalls.Timer_GetTimeScale();
        }
    }
}
