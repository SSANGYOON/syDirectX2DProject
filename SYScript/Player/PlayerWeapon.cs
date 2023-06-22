using SY;
using SYScript;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class PlayerWeapon : Weapon
    {
        private TransformAnimatorComponent m_Animator;
        private Entity player;

        private PlayerWeaponData weaponData = null;
        private AudioSource m_AudioSource;

        private float attackTime = 0f;

        public bool Active
        {
            get { return active; }
            set { active = value; attackTime = 0f; }
        }

        public PlayerWeaponData Data
        {
            get { return weaponData; }
            set {
                PlayerWeaponData.weaponType prevType = PlayerWeaponData.weaponType.None;
                weaponData = value;
                m_Animator.Stop();
                if (weaponData != null)
                {
                    if (weaponData.Type != prevType)
                    {
                        switch (weaponData.Type)
                        {
                            case PlayerWeaponData.weaponType.OneHand:
                                m_BoxCollider.Size = new Vector2(3, 15);
                                m_BoxCollider.Offset = new Vector2(0, 21);
                                break;

                            case PlayerWeaponData.weaponType.Dagger:
                                m_BoxCollider.Size = new Vector2(2, 10);
                                m_BoxCollider.Offset = new Vector2(0, 4);
                                break;
                            default:
                                break;
                        }
                    }
                    if (weaponData.Type == PlayerWeaponData.weaponType.Shield)
                        m_SpriteRenderer.Texture = weaponData.spritePath2;
                    else
                        m_SpriteRenderer.Texture = weaponData.spritePath;
                }
            }
        }

        public void OnCreate()
        {
            m_Animator = GetComponent<TransformAnimatorComponent>();
            m_SpriteRenderer = GetComponent<SpriteRendererComponent>();
            m_BoxCollider = GetComponent<BoxCollider2DComponent>();

            player = FindEntityByName("Player");
            m_AudioSource = GetComponent<AudioSource>();
        }

        public void Attack(float at = 0.0f)
        {
            if (m_Animator != null && weaponData != null)
            {
                m_Animator.Play(Enum.GetName(typeof(PlayerWeaponData.weaponType), weaponData.Type), at);
                attackTime = 0f;
                
            }
        }

        void OnUpdate(float ts)
        {
            if (active)
            { 
                if(attackTime < 0.4f && attackTime + ts > 0.4f)
                    m_AudioSource.Play("assets\\soundClip\\Woosh_Sword_Normal_02.wav", false);

                attackTime += ts;
            }
        }

        public override void ApplyDamage(Character enemy) {
            enemy.ReceiveDamage(weaponData.Damage);
        }

        public override void OnGuarded(Character enemy)
        {
            m_AudioSource.Play("assets\\soundClip\\IMPACT_Metal_Hit_Large_Metal_02_mono.wav", false);
        }

        void OnTriggerEnter(ref Collision2D collsion)
        {
            if (weaponData != null)
            {
                if (weaponData.Type == PlayerWeaponData.weaponType.Shield)
                    return;
                if (active)
                {
                    Entity opp = new Entity(collsion.entityID);
                    Character enemy = opp.As<Character>();
                    if (enemy == null)
                        return;
                    if (enemy.Invisible)
                        return;

                    bool guard = enemy.GuardCheck(this);
                    if (guard)
                    {
                        OnGuarded(enemy);
                        player.As<Character>().OnBlocked();
                        enemy.OnGuarded(this);
                    }
                    else
                    {
                        OnAttack(enemy);
                        ApplyDamage(enemy);
                        enemy.OnAttacked(this);
                    }
                }
            }
        }
    }
}
