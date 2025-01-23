using ServerCore;
using System;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf;
using Protocol;
using UnityEngine.SceneManagement;
using Unity.VisualScripting;

class PacketHandler
{

    public static void S_PING_Handler(PacketSession session, IMessage packet)
    {
        // S_PING 패킷 파싱
        S_PING pingPacket = packet as S_PING;

        // 서버에서 보낸 타임스탬프 확인
        ulong serverTimestamp = pingPacket.TimeStamp;

        // 왕복 시간(RTT) 계산
        ulong currentTimestamp = (ulong)(DateTime.UtcNow - DateTime.UnixEpoch).TotalMilliseconds;
        ulong roundTripTime = currentTimestamp - serverTimestamp;

        Debug.Log($"[Client] Received S_PONG. RTT: {roundTripTime} ms");
    }

    public static void S_USER_STATUS_Handler(PacketSession session, IMessage packet)
    {

    }

    public static void S_ENTER_ROOM_Handler(PacketSession session, IMessage packet)
    {
        S_ENTER_ROOM enterGamePacket = packet as S_ENTER_ROOM;

        Managers.Object.Add(enterGamePacket.Player, myPlayer: true);
        Managers.Object.MyPlayer.GetComponent<CharacterController>().enabled = false;
        Managers.Object.MyPlayer.transform.position = new Vector3(0, 0, 0);
        Managers.Object.MyPlayer.GetComponent<CharacterController>().enabled = true;

        C_POS_SYNC posSyncPacket = new C_POS_SYNC
        {
            PosInfo = new PositionInfo
            {
                PosX = 0,
                PosZ = 0,
            },
        };
        Managers.Network.Send(posSyncPacket);  // 서버로 위치 패킷 전송

        Managers.Map.ChangeMap(enterGamePacket.MapId);
    }

    public static void S_LEAVE_ROOM_Handler(PacketSession session, IMessage packet)
    {
        S_LEAVE_ROOM leaveGamePacket = packet as S_LEAVE_ROOM;

        Managers.Object.Remove(leaveGamePacket.ObjectId);
    }

    public static void S_SPAWN_Handler(PacketSession session, IMessage packet)
    {
        S_SPAWN spawnPacket = packet as S_SPAWN;
        foreach (ObjectInfo obj in spawnPacket.Objects)
        {
            Managers.Object.Add(obj, myPlayer: false);
        }
    }

    public static void S_DESPAWN_Handler(PacketSession session, IMessage packet)
    {
        S_DESPAWN despawnPacket = packet as S_DESPAWN;
        foreach (int id in despawnPacket.ObjectIds)
        {
            Managers.Object.Remove(id);
        }
    }

    public static void S_DIE_Handler(PacketSession session, IMessage packet)
    {
        S_DIE diePacket = packet as S_DIE;

        GameObject go = Managers.Object.FindById(diePacket.ObjectId);
        if (go == null)
            return;

        CreatureController cc = go.GetComponent<CreatureController>();
        if (cc != null)
        {
            cc.Hp = 0;
            cc.OnDead();
        }
    }

    public static void S_MOVE_Handler(PacketSession session, IMessage packet)
    {
        S_MOVE movePacket = packet as S_MOVE;

        if (Managers.Object.MyPlayer.Id == movePacket.ObjectId)
        {
            // 서버에서 보낸 타임스탬프 확인
            ulong serverTimestamp = movePacket.TimeStamp;

            // 왕복 시간(RTT) 계산
            ulong currentTimestamp = (ulong)(DateTime.UtcNow - DateTime.UnixEpoch).TotalMilliseconds;
            ulong roundTripTime = currentTimestamp - serverTimestamp;

            Debug.Log($"[Client] Received S_PONG. RTT: {roundTripTime} ms");
        }

        GameObject go = Managers.Object.FindById(movePacket.ObjectId);
        if (go == null)
            return;


        BaseController bc = go.GetComponent<BaseController>();
        if (bc == null)
            return;

        bc.PosInfo = movePacket.PosInfo;

    }

    public static void S_ATTACK_Handler(PacketSession session, IMessage packet)
    {
        S_ATTACK pkt = packet as S_ATTACK;

        GameObject go = Managers.Object.FindById(pkt.AttackerId);
        if (go == null)
            return;

        CreatureController cc = go.GetComponent<CreatureController>();
        if (cc != null)
        {
            cc.Attack(pkt.DirX, pkt.DirZ);
        }
    }

    public static void S_UPDATE_RESOURCE_Handler(PacketSession session, IMessage packet)
    {
        S_UPDATE_RESOURCE changePacket = packet as S_UPDATE_RESOURCE;

        GameObject go = Managers.Object.FindById(changePacket.ObjectId);
        if (go == null)
            return;

        CreatureController cc = go.GetComponent<CreatureController>();
        if (cc != null)
        {
            if (changePacket.Type == ResourceType.Hp)
            {
                cc.Hp = changePacket.NowValue;
                bool isMyPlayer = cc is MyPlayerController;
                Managers.UI.UpdateHealthBar(changePacket.ObjectId, cc.Hp, cc.Stat.MaxHp, isMyPlayer);

                if (changePacket.ChangeValue < 0)
                {
                    Managers.UI.ShowDamageText(changePacket.ChangeValue * -1, cc.transform.position);
                }
            }
            else if (changePacket.Type == ResourceType.Mp)
            {
                cc.Mp = changePacket.NowValue;
                bool isMyPlayer = cc is MyPlayerController;
                Managers.UI.UpdateManaBar(changePacket.ObjectId, cc.Mp, cc.Stat.MaxMp, isMyPlayer);

            }

        }
    }

    public static void S_LOGIN_Handler(PacketSession session, IMessage packet)
    {
        S_LOGIN pkt = packet as S_LOGIN;

        if (pkt.Success == true)
        {
            AsyncOperation asyncLoad = SceneManager.LoadSceneAsync("02. GameServer");

            asyncLoad.completed += (operation) =>
            {
                // 씬 로드가 완료된 후 실행
                Managers.InitAfterLogin();
                Managers.Inven.Gold = pkt.PlayerInfo.GoldAmount;
                Managers.UI.ShowGold();
            };
        }
    }


    public static void S_ITEM_LIST_Handler(PacketSession session, IMessage packet)
    {
        S_ITEM_LIST itemList = packet as S_ITEM_LIST;

        Managers.Inven.Clear();
        foreach (ItemInfo itemInfo in itemList.Items)
        {
            Item item = Item.MakeItem(itemInfo);
            Managers.Inven.Add(item);
        }
        Managers.UI.RefreshInventory();
    }

    public static void S_ADD_ITEM_Handler(PacketSession session, IMessage packet)
    {
        S_ADD_ITEM pkt = packet as S_ADD_ITEM;

        foreach (ItemInfo itemInfo in pkt.Items)
        {
            Item item = Item.MakeItem(itemInfo);
            Managers.Inven.Add(item);
            Managers.UI.ShowItemLog(itemInfo);
        }
        Managers.UI.RefreshInventory();
    }

    public static void S_CHANGE_CURRENCY_Handler(PacketSession session, IMessage packet)
    {
        S_CHANGE_CURRENCY pkt = packet as S_CHANGE_CURRENCY;

        if (pkt.CurrencyType == CurrencyType.Gold)
        {
            Managers.Inven.Gold = pkt.TotalAmount;
            Managers.UI.ShowGold();
        }

        if (pkt.ChangeAmount > 0)
        {
            Managers.UI.ShowCurrencyLog(pkt.CurrencyType, pkt.ChangeAmount);
        }
    }

    public static void S_SKILL_Handler(PacketSession session, IMessage packet)
    {
        S_SKILL pkt = packet as S_SKILL;

        if (!pkt.Success)
            return;

        Managers.Skill.ShowSkill(pkt);
    }

    public static void S_CHANGE_ITEM_INFO_Handler(PacketSession session, IMessage packet)
    {
        S_CHANGE_ITEM_INFO pkt = packet as S_CHANGE_ITEM_INFO;

        foreach (ItemInfo itemInfo in pkt.Items)
        {
            Item item = Item.MakeItem(itemInfo);

            if (itemInfo.Equipped)
            {
                Managers.Inven.EquippedItem[item.ItemType] = item;
            }

            Managers.Inven.Change(item);
        }

        foreach (Item item in Managers.Inven.EquippedItem.Values)
        {
            if (Managers.Inven.Items[item.ItemDbId].Equipped == false)
            {
                Managers.Inven.EquippedItem.Remove(item.ItemType);
                break;
            }
        }
        Managers.UI.SetStatUI();
        Managers.UI.RefreshInventory();
    }

    public static void S_REMOVE_ITEM_Handler(PacketSession session, IMessage packet)
    {
        S_REMOVE_ITEM pkt = packet as S_REMOVE_ITEM;
        Managers.Inven.Remove(pkt.RemovedItemDbID);
        Managers.UI.RefreshInventory();
    }

    public static void S_MARKET_ITEM_LIST_Handler(PacketSession session, IMessage packet)
    {
        S_MARKET_ITEM_LIST pkt = packet as S_MARKET_ITEM_LIST;
        Managers.Market.HandleMarketItemList(pkt);
    }

    public static void S_PURCHASE_MARKET_ITEM_Handler(PacketSession session, IMessage packet)
    {
        S_PURCHASE_MARKET_ITEM pkt = packet as S_PURCHASE_MARKET_ITEM;

        Item item = Item.MakeItem(pkt.Items);
        Managers.Inven.Add(item);
        Managers.UI.ShowItemLog(pkt.Items);
        Managers.Market.MarketItems.Remove(item.ItemDbId);
        Managers.UI.RefreshInventory();
        Managers.UI.RefreshMarketUI(Managers.Market.MarketItems.Values);
    }

    public static void S_CHAT_Handler(PacketSession session, IMessage packet)
    {
        S_CHAT pkt = packet as S_CHAT;
        Managers.Chat.HandleChat(pkt);
    }

}
