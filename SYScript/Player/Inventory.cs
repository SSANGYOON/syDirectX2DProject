﻿using SY;
using SYScript;
using System;
using System.Collections.Generic;
using System.Diagnostics.Eventing.Reader;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Sandbox.Player;

namespace Sandbox
{
    
    public class Inventory : Entity
    {
        public Entity panalData;
        private Entity previewSlot;
        public Player player;

        public InventoryData items;

        private string equipableTexturePath = "assets\\textures\\ui\\Equippable.png";
        private string equipedTexturePath = "assets\\textures\\ui\\Equipped.png";

        private Entity[] icons;
        private Entity[] panels;

        private Entity xWeaponView;
        private Entity zWeaponView;

        private int currentPanelIndex = 0;

        private int CurrentPanelIndex
        { 
            get { return currentPanelIndex; }
            set
            {
                int currentCategoryItemNum = items.itemDatas[(int)CurrentCategory].Count;

                int nextIndex;
                nextIndex = value;

                if (nextIndex < 0)
                    nextIndex = Math.Max(currentCategoryItemNum - 1,0);
                if (nextIndex >= currentCategoryItemNum)
                    nextIndex = 0;

                PanelComponent panelComp = panels[currentPanelIndex].GetComponent<PanelComponent>();
                panelComp.TintColor = new Vector4(1, 1, 1, 0f);
                    
                PanelComponent newPanelComp = panels[nextIndex].GetComponent<PanelComponent>();
                newPanelComp.TintColor = new Vector4(1, 1, 1, 0.51f);

                SlotComponent slot = previewSlot.GetComponent<SlotComponent>();

                if(currentCategoryItemNum > 0)
                    slot.Texture = items.itemDatas[(int)CurrentCategory][nextIndex].spritePath;
                else
                    slot.Texture= "";

                currentPanelIndex = nextIndex;
            }
        }

        private InventoryData.CATEGORY currentCategory = InventoryData.CATEGORY.Weapon;
        private InventoryData.CATEGORY CurrentCategory
        {
            get { return currentCategory; }
            set {
                var iconPrev = icons[(int)currentCategory].GetComponent<IconComponent>();
                iconPrev.TintColor = new Vector4(0.5f, 0.5f, 0.5f, 1);

                currentCategory = value;
                var iconCurrent = icons[(int)currentCategory].GetComponent<IconComponent>();
                iconCurrent.TintColor = new Vector4(1, 1, 1, 1);

                int currentCategoryItemNum = items.itemDatas[(int)currentCategory].Count;

                for (int i = 0; i < currentCategoryItemNum; i++)
                {
                    panels[i].Activate();
                }

                for (int i = currentCategoryItemNum; i < 8; i++)
                {
                    panels[i].Pause();
                }

                CurrentPanelIndex = 0;
            }
        }

        void OnCreate()
        {
            items = new InventoryData();
            previewSlot = FindEntityByName("ItemPreviewSlot");

            icons = new Entity[4];
            
            for (int i = 0; i < icons.Length; i++)
            {
                icons[i] = FindEntityByName(Enum.GetName(typeof(InventoryData.CATEGORY), (InventoryData.CATEGORY)i) + "Icon");
            }
            currentCategory = InventoryData.CATEGORY.Weapon;

            panels = new Entity[8];
            for (int i = 0; i < 8; i++)
            {
                panels[i] = panalData.Instantiate(new Vector3(-90, 45 - 16 * i, -1), ID);
                var panelComp = panels[i].GetComponent<PanelComponent>();
                panelComp.TintColor = new Vector4(0, 0, 0, 0);
                Entity equipedIcon = panels[i].GetChild("equip slot");
                var iconComp = equipedIcon.GetComponent<IconComponent>();
                iconComp.Texture = equipableTexturePath;
            }
            CurrentCategory = currentCategory;

            xWeaponView = FindEntityByName("xWeaponView");
            zWeaponView = FindEntityByName("zWeaponView");
        }

        void OnUpdate(float ts)
        {
            if(player == null)
            {
                player = FindEntityByName("Player").As<Player>();
            }

            if(Input.IsKeyDown(KeyCode.Right))
            {
                CurrentCategory = (InventoryData.CATEGORY)(((int)CurrentCategory + 1) % icons.Length);
            }

            if (Input.IsKeyDown(KeyCode.Left))
            {
                CurrentCategory = (InventoryData.CATEGORY)(((int)CurrentCategory - 1 + icons.Length) % icons.Length);
            }

            if (Input.IsKeyDown(KeyCode.Up)) { CurrentPanelIndex = CurrentPanelIndex - 1; }
            if (Input.IsKeyDown(KeyCode.Down)) { CurrentPanelIndex = CurrentPanelIndex + 1; }

            if(Input.IsKeyDown(KeyCode.Z)) {
                if (CurrentCategory == InventoryData.CATEGORY.Weapon)
                {
                    
                    var weaponData = items.itemDatas[(int)CurrentCategory][currentPanelIndex] as PlayerWeaponData;
                    var xslot = xWeaponView.GetComponent<SlotComponent>();
                    var zslot = zWeaponView.GetComponent<SlotComponent>();
                    if (items.itemDatas[(int)CurrentCategory].Count > 0)
                    {
                        if (player.zWeapon.Data == weaponData && weaponData != null)
                        {
                            player.zWeapon.Data = null;
                            zslot.Texture = "";
                        }
                        else if (weaponData != null)
                        {
                            player.zWeapon.Data = weaponData;
                            zslot.Texture = weaponData.spritePath;
                            if (player.zWeapon.Data == player.xWeapon.Data)
                            {
                                player.xWeapon.Data = null;
                                xslot.Texture = "";
                            }
                        }
                    }
                }
            }

            if (Input.IsKeyDown(KeyCode.X))
            {
                if (CurrentCategory == InventoryData.CATEGORY.Weapon)
                {
                    var weaponData = items.itemDatas[(int)CurrentCategory][currentPanelIndex] as PlayerWeaponData;
                    var xslot = xWeaponView.GetComponent<SlotComponent>();
                    var zslot = zWeaponView.GetComponent<SlotComponent>();
                    if (items.itemDatas[(int)CurrentCategory].Count > 0)
                    {
                        if (player.xWeapon.Data == weaponData && weaponData != null)
                        {
                            player.xWeapon.Data = null;
                            xslot.Texture = "";
                        }
                        else if (weaponData != null)
                        {
                            player.xWeapon.Data = weaponData;
                            xslot.Texture = weaponData.spritePath;
                            if (player.xWeapon.Data == player.zWeapon.Data)
                            {
                                player.zWeapon.Data = null;
                                zslot.Texture = "";
                            }
                        }
                    }
                }
            }

            if(CurrentCategory == InventoryData.CATEGORY.Weapon)
            {
                var weapon = items.itemDatas[(int)CurrentCategory];

                for (int i = 0; i < weapon.Count; i++)
                {
                    Entity slot = panels[i].GetChild("equip slot");
                    var slotComp = slot.GetComponent<IconComponent>();
                    if (weapon[i] as PlayerWeaponData == player.zWeapon.Data || weapon[i] as PlayerWeaponData == player.xWeapon.Data)
                        slotComp.Texture = equipedTexturePath;
                    else
                        slotComp.Texture = equipableTexturePath;
                }
            }
        }

        void OnPaused()
        {
            Timer.SetTimeScale(1.0f);
        }

        void OnActivated()
        {
            Timer.SetTimeScale(0.0f);
        }
    }
}
