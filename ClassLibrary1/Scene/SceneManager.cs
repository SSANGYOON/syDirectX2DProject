using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SY
{
    public class SceneManager
    {
        private static string reservedScenePath = string.Empty;
        public static void ReserveNextScene(string path)
        {
            reservedScenePath = path;
        }
        public static void LoadReservedScene()
        { 
            InternalCalls.SceneManager_LoadSceneAsync(reservedScenePath);
            Console.WriteLine(reservedScenePath);
        }
        public static void LoadScene(string path)
        { 
            InternalCalls.SceneManager_LoadScene(path);
        }

        public static void LoadSceneAsync(string path)
        {
            InternalCalls.SceneManager_LoadSceneAsync(path);
        }
    }
}
