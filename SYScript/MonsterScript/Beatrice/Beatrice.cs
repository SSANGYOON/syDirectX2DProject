using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Beatrice : Character
    {
        private SpriteAnimatorComponent animator;
        private SpriteRendererComponent renderer;
        private ParticleSystem particleSystem;
        private AudioSource audioSource;

        public enum BState
        {
            Initial,
            TelePort,
        }

        private BState _state;

        public BState State
        {
            get { return _state; }
            set
            {
                _state = value; stateTime = 0f;
                switch (_state)
                {
                    case BState.Initial:
                        animator.Play("IdleB");
                        break;

                    case BState.TelePort:
                        animator.Play("Casting1B");
                        audioSource.Play("assets\\soundClip\\Holy (1).wav");
                        break;
                }
            }

        }

        void OnCreate()
        {
            animator = GetComponent<SpriteAnimatorComponent>();
            renderer = GetComponent<SpriteRendererComponent>();
            particleSystem = FindEntityByName("BeatriceParticle").GetComponent<ParticleSystem>();
            audioSource = GetComponent<AudioSource>();
            State = BState.Initial;
        }

        void OnUpdate(float ts)
        {
            if (State == BState.TelePort)
            {
                if (stateTime < 0.5f && stateTime + ts > 0.5f)
                {
                    animator.Stop();
                    renderer.tile = new Vector2(1, 1);
                    particleSystem.State = (uint)ParticleSystem.ParticleState.UPDATE_ONLY;
                }

                if (stateTime < 1f && stateTime + ts > 1f)
                    Pause();
            }
            stateTime += ts;
        }
    }
}
