using System;
using System.Collections.Generic;
using Google.Protobuf;
using Protocol;
using ServerCore;

class PacketManager
{
    #region Singleton
    static PacketManager _instance = new PacketManager();
    public static PacketManager Instance { get { return _instance; } }
    #endregion

    PacketManager()
    {
        Register();
    }

    public enum PacketID
    {
        C_LOGIN = 1000,
        C_SIGN_UP = 1001,
        S_LOGIN = 1002,
        C_ENTER_GAME = 1003,
        C_ENTER_ROOM = 1004,
        S_ENTER_ROOM = 1005,
        C_LEAVE_GAME = 1006,
        S_LEAVE_ROOM = 1007,
        S_SPAWN = 1008,
        S_DESPAWN = 1009,
        C_MOVE = 1010,
        S_MOVE = 1011,
        C_POS_SYNC = 1012,
        C_ATTACK = 1013,
        S_ATTACK = 1014,
        S_UPDATE_RESOURCE = 1015,
        S_DIE = 1016,
        S_PING = 1017,
        C_PONG = 1018,
        S_USER_STATUS = 1019,
        S_ITEM_LIST = 1020,
        S_ADD_ITEM = 1021,
        S_CHANGE_CURRENCY = 1022,
        C_SKILL = 1023,
        S_SKILL = 1024,
        C_USE_ITEM = 1025,
        S_CHANGE_ITEM_INFO = 1026,
        C_DROP_ITEM = 1027,
        S_REMOVE_ITEM = 1028,
        C_REQUEST_MARKET_ITEM_LIST = 1029,
        S_MARKET_ITEM_LIST = 1030,
        C_PURCHASE_MARKET_ITEM = 1031,
        C_REGISTER_MARKET_ITEM = 1032,
        S_PURCHASE_MARKET_ITEM = 1033,
        C_CHAT = 1034,
        S_CHAT = 1035,
        C_WHISPER = 1036,
    }

    Dictionary<ushort, Action<PacketSession, ArraySegment<byte>, ushort>> _onRecv = new Dictionary<ushort, Action<PacketSession, ArraySegment<byte>, ushort>>();
    Dictionary<ushort, Action<PacketSession, IMessage>> _handler = new Dictionary<ushort, Action<PacketSession, IMessage>>();

    public Action<PacketSession, IMessage, ushort> CustomHandler { get; set; }

    public void Register()
    {
        _onRecv.Add((ushort)1002, MakePacket <S_LOGIN>);
        _handler.Add((ushort)1002, PacketHandler.S_LOGIN_Handler);
        _onRecv.Add((ushort)1005, MakePacket <S_ENTER_ROOM>);
        _handler.Add((ushort)1005, PacketHandler.S_ENTER_ROOM_Handler);
        _onRecv.Add((ushort)1007, MakePacket <S_LEAVE_ROOM>);
        _handler.Add((ushort)1007, PacketHandler.S_LEAVE_ROOM_Handler);
        _onRecv.Add((ushort)1008, MakePacket <S_SPAWN>);
        _handler.Add((ushort)1008, PacketHandler.S_SPAWN_Handler);
        _onRecv.Add((ushort)1009, MakePacket <S_DESPAWN>);
        _handler.Add((ushort)1009, PacketHandler.S_DESPAWN_Handler);
        _onRecv.Add((ushort)1011, MakePacket <S_MOVE>);
        _handler.Add((ushort)1011, PacketHandler.S_MOVE_Handler);
        _onRecv.Add((ushort)1014, MakePacket <S_ATTACK>);
        _handler.Add((ushort)1014, PacketHandler.S_ATTACK_Handler);
        _onRecv.Add((ushort)1015, MakePacket <S_UPDATE_RESOURCE>);
        _handler.Add((ushort)1015, PacketHandler.S_UPDATE_RESOURCE_Handler);
        _onRecv.Add((ushort)1016, MakePacket <S_DIE>);
        _handler.Add((ushort)1016, PacketHandler.S_DIE_Handler);
        _onRecv.Add((ushort)1017, MakePacket <S_PING>);
        _handler.Add((ushort)1017, PacketHandler.S_PING_Handler);
        _onRecv.Add((ushort)1019, MakePacket <S_USER_STATUS>);
        _handler.Add((ushort)1019, PacketHandler.S_USER_STATUS_Handler);
        _onRecv.Add((ushort)1020, MakePacket <S_ITEM_LIST>);
        _handler.Add((ushort)1020, PacketHandler.S_ITEM_LIST_Handler);
        _onRecv.Add((ushort)1021, MakePacket <S_ADD_ITEM>);
        _handler.Add((ushort)1021, PacketHandler.S_ADD_ITEM_Handler);
        _onRecv.Add((ushort)1022, MakePacket <S_CHANGE_CURRENCY>);
        _handler.Add((ushort)1022, PacketHandler.S_CHANGE_CURRENCY_Handler);
        _onRecv.Add((ushort)1024, MakePacket <S_SKILL>);
        _handler.Add((ushort)1024, PacketHandler.S_SKILL_Handler);
        _onRecv.Add((ushort)1026, MakePacket <S_CHANGE_ITEM_INFO>);
        _handler.Add((ushort)1026, PacketHandler.S_CHANGE_ITEM_INFO_Handler);
        _onRecv.Add((ushort)1028, MakePacket <S_REMOVE_ITEM>);
        _handler.Add((ushort)1028, PacketHandler.S_REMOVE_ITEM_Handler);
        _onRecv.Add((ushort)1030, MakePacket <S_MARKET_ITEM_LIST>);
        _handler.Add((ushort)1030, PacketHandler.S_MARKET_ITEM_LIST_Handler);
        _onRecv.Add((ushort)1033, MakePacket <S_PURCHASE_MARKET_ITEM>);
        _handler.Add((ushort)1033, PacketHandler.S_PURCHASE_MARKET_ITEM_Handler);
        _onRecv.Add((ushort)1035, MakePacket <S_CHAT>);
        _handler.Add((ushort)1035, PacketHandler.S_CHAT_Handler);

    }

    public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer)
    {
        ushort count = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        count += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
        count += 2;

        if (_onRecv.TryGetValue(id, out var action))
            action.Invoke(session, buffer, id);
    }

    void MakePacket<T>(PacketSession session, ArraySegment<byte> buffer, ushort id) where T : IMessage, new()
    {
        T pkt = new T();
        pkt.MergeFrom(buffer.Array, buffer.Offset + 4, buffer.Count - 4);

        if (CustomHandler != null)
        {
            CustomHandler.Invoke(session, pkt, id);
        }
        else
        {
            if (_handler.TryGetValue(id, out var action))
                action.Invoke(session, pkt);
        }
    }

    public Action<PacketSession, IMessage> GetPacketHandler(ushort id)
    {
        if (_handler.TryGetValue(id, out var action))
            return action;
        return null;
    }
}