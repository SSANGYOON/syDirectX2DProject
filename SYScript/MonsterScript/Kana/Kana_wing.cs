using SY;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Kana_wing : Entity
    {
        SpriteAnimatorComponent animator;

        enum wingState
        { 
            wing1, wing2, wing3, wing4
        }

        wingState _state;

        float _stateTime = 0;
        wingState State
        {
            get { return _state; }
            set
            {
                switch (value)
                {
                    case wingState.wing1:
                        animator.Play("Kana_Wing1");
                        Translation = new Vector3(17, 26.5F, 1);
                        break;
                    case wingState.wing2:
                        animator.Play("Kana_Wing2");
                        Translation = new Vector3(15, -3, 1);
                        break;
                    case wingState.wing3:
                        animator.Play("Kana_Wing3");
                        Translation = new Vector3(7, 0, 1);
                        break;
                    case wingState.wing4:
                        animator.Play("Kana_Wing4");
                        Translation = new Vector3(14, 31.5f, 1);
                        break;
                }
                _stateTime = 0f;
                _state = value;
            }
        }
        void OnCreate()
        {
            animator = GetComponent<SpriteAnimatorComponent>();
            State = wingState.wing1;
        }

        void OnUpdate(float ts)
        {
            switch (State)
            {
                case wingState.wing1:
                    if(_stateTime>0.4f)
                        State = wingState.wing2;
                    break;
                case wingState.wing2:
                    if (_stateTime > 0.4f)
                        State = wingState.wing3;
                    break;
                case wingState.wing3:
                    if (_stateTime > 0.5f)
                        State = wingState.wing4;
                    break;
                case wingState.wing4:
                    if (_stateTime > 0.3f)
                        State = wingState.wing1;
                    break;
            }
            _stateTime += ts;
        }
    }
}
