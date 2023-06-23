using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Resources;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Sandbox.Player;

namespace Sandbox
{
    public class TitleSceneController : Entity
    {
        UIText start;
        UIText end;

        AudioSource menuAudio;
        private string LoadingScenePath = "Resources\\assets\\Scenes\\LoadingScene.hazel";

        bool _currentMenu;

        bool CurrentMenu
        { 
            get { return _currentMenu; }
            set { _currentMenu = value;
                if (_currentMenu)
                {
                    start.Color = new Vector4(1, 1, 1, 0.5f);
                    end.Color = new Vector4(1, 1, 1, 1f);
                }
                else {
                    start.Color = new Vector4(1, 1, 1, 1);
                    end.Color = new Vector4(1, 1, 1, 0.5f);
                }
            }
        }

        void OnCreate()
        {
            Entity player = FindEntityByName("Heroine");
            Entity bush = FindEntityByName("Bush");
            Entity ropes = FindEntityByName("Ropes");
            Entity tree = FindEntityByName("Tree");

            

            if (player != null)
            {
                var anim = player.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Heroine");

                menuAudio = player.GetComponent<AudioSource>();
            }

            if (bush != null)
            {
                var anim = bush.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Bush");
            }

            if (ropes != null)
            {
                var anim = ropes.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Ropes");
            }

            if (tree != null)
            {
                var anim = tree.GetComponent<SpriteAnimatorComponent>();
                anim.Play("Tree");
            }

            start = FindEntityByName("StartMenu").GetComponent<UIText>();
            end = FindEntityByName("EndMenu").GetComponent<UIText>();

            CurrentMenu = false;

            GetComponent<AudioSource>().Play("assets\\soundClip\\7050905_MotionElements_atelier-in-the-forest.wav", true);
        }

        void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(KeyCode.Down) || Input.IsKeyDown(KeyCode.Up)) {
                CurrentMenu = !CurrentMenu;
                menuAudio.Play("assets\\soundClip\\Click_Standard_00.wav", false);
            }

            if (!CurrentMenu && Input.IsKeyDown(KeyCode.Z))
            {
                GetComponent<AudioSource>().Stop();
                SceneManager.ReserveNextScene("Resources\\assets\\Scenes\\Castle.hazel");
                SceneManager.LoadScene(LoadingScenePath);
            }
            
        }
    }
}
