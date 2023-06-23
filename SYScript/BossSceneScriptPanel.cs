using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class BossSceneScriptPanel : Entity
    {
        public string[] scripts;

        private UIText scriptText;
        private AudioSource effectSound;

        private IconComponent playerIcon;
        private IconComponent oppIcon;
        float stateTime = 0f;

        private uint _currentScript = 0;
        private bool _playerTurn = false;

        private string[] playerTextures;
        private string[] oppTextures;
        private bool[] turns;


        uint CurrentScript
        { 
            get { return _currentScript; }
            set { _currentScript = value; stateTime = 0f;
                if (CurrentScript < scripts.Length)
                {
                    PlayerTurn = turns[CurrentScript];
                    playerIcon.Texture = playerTextures[CurrentScript];
                    oppIcon.Texture = oppTextures[CurrentScript];
                }
            }
        }

        bool PlayerTurn
        {
            get { return _playerTurn; }
            set { _playerTurn = value;
                if (value)
                {
                    playerIcon.TintColor = new Vector4(1, 1, 1, 1);
                    oppIcon.TintColor = new Vector4(0.5f, 0.5f, 0.5f, 1);
                }

                else {
                    playerIcon.TintColor = new Vector4(0.5f, 0.5f, 0.5f, 1); 
                    oppIcon.TintColor = new Vector4(1, 1, 1, 1);
                }
            }
        }

        void OnCreate()
        {
            scripts = new string[9];
            turns = new bool[9];

            playerTextures = new string[9];
            oppTextures = new string[9];

            scripts[0] = "아아... 여기까지 쫓아왔구나. 실패작들과 싸우다가 죽을 줄 알았는데 말이야.";
            turns[0] = false;
            playerTextures[0] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[0] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scripts[1] = "그 아이들도 모두 당신이 만들어낸 건가요?";
            turns[1] = true;
            playerTextures[1] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[1] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scripts[2] = "물론이지. 모두 너랑 똑같은 얼굴을 하고 있지 않았어?";
            turns[2] = false;
            playerTextures[2] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[2] = "assets\\textures\\NpcSummonerPortraitSigh.png";

            scripts[3] = "자랑스러워해도 좋아. 그것들은 모두 실패작들이지만 너만큼은 유일한 완성품이니까.";
            turns[3] = false;
            playerTextures[3] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[3] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scripts[4] = "도대체 왜...";
            turns[4] = true;
            playerTextures[4] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[4] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scripts[5] = "이미 말하지 않았나? 소환한 녀석들을 처리하기 위해 너를 만들었어. 그 녀석들은 네가 되다만 것들이지.";
            turns[5] = false;
            playerTextures[5] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[5] = "assets\\textures\\NpcSummonerPortraitSigh.png";

            scripts[6] = "제가... 당신에 의해 만들어진 존재라면 저는 대체 앞으로 어떻게 해야 하는 거죠?";
            turns[6] = true;
            playerTextures[6] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[6] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scripts[7] = "그거라면 간단해. 마왕 소환의 준비는 모두 끝났으니까 네가 계속 살아있을 필요는 없어.";
            turns[7] = false;
            playerTextures[7] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[7] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scripts[8] = "그러니까 이제 사라질 시간이야.";
            turns[8] = false;
            playerTextures[8] = "assets\\textures\\PlayerPortraitAh.png";
            oppTextures[8] = "assets\\textures\\NpcSummonerPortraitDefault.png";

            scriptText = FindEntityByName("ScriptText").GetComponent<UIText>();
            effectSound = GetComponent<AudioSource>();

            playerIcon = FindEntityByName("PlayerIcon").GetComponent<IconComponent>();
            oppIcon = FindEntityByName("OppIcon").GetComponent<IconComponent>();

            PlayerTurn = turns[0];
        }

        void OnUpdate(float ts)
        {
            if (CurrentScript < scripts.Length)
            {
                if ((int)(stateTime * 10) <= scripts[CurrentScript].Length)
                {
                    if ((int)(stateTime * 10) < (int)(10 * (stateTime + ts)) && 10 * stateTime > 1)
                    {
                        scriptText.Text = scripts[CurrentScript].Substring(0, (int)(stateTime * 10));
                        effectSound.Play("assets\\soundClip\\Click_Soft_01.wav");
                    }
                }
            }
            else
            {
                Entity boss = FindEntityByName("DarkLady");
                var cam = FindEntityByName("MainCamera").As<Camera>();
                cam.Cinematic = false;
                if (boss != null)
                {
                    var bossCon = boss.As<BossController>();
                    bossCon.bossState = BossController.BossState.Chain;
                    FindEntityByName("Beatrice").As<Beatrice>().State = Beatrice.BState.TelePort;
                    Pause();
                }
            }

            if (Input.IsKeyDown(KeyCode.V))
            {
                CurrentScript = _currentScript + 1;
                
            }

            stateTime += ts;
        }
    }
}
