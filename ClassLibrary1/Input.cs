namespace SY
{
    public class Input
    {
        enum KeyState
        {
            NONE,
            PRESS,
            DOWN,
            UP,
        }
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_GetKeyState(keycode) == (int)KeyState.DOWN;
        }
        public static bool IsKeyPressed(KeyCode keycode)
        {
            return InternalCalls.Input_GetKeyState(keycode) == (int)KeyState.PRESS;
        }
        public static bool IsKeyUp(KeyCode keycode)
        {
            return InternalCalls.Input_GetKeyState(keycode) == (int)KeyState.UP;
        }
        public static bool IsKeyNone(KeyCode keycode)
        {
            return InternalCalls.Input_GetKeyState(keycode) == (int)KeyState.NONE;
        }
    }
}