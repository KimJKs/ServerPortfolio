using Google.Protobuf;
using Protocol;
using ServerCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using static PacketManager;


enum ServerType
{
    Main,
    Chat
}

class ServerSession : PacketSession
{
    public void Send(IMessage packet, ServerType servertype)
    {
        string msgName = packet.Descriptor.Name;

        // PacketID와 매칭
        if (!Enum.TryParse(typeof(PacketID), msgName, out var packetIdEnum))
        {
            Debug.LogError($"Invalid packet name: {msgName}");
            return;
        }
        ushort packetId = (ushort)(PacketID)packetIdEnum;


        ushort size = (ushort)packet.CalculateSize();
        byte[] sendBuffer = new byte[size + 4];
        Array.Copy(BitConverter.GetBytes((ushort)(size + 4)), 0, sendBuffer, 0, sizeof(ushort));
        Array.Copy(BitConverter.GetBytes(packetId), 0, sendBuffer, 2, sizeof(ushort));
        Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);
        Send(new ArraySegment<byte>(sendBuffer));
    }


    public override void OnConnected(EndPoint endPoint)
    {

        PacketManager.Instance.CustomHandler = (s, m, i) =>
            {
                PacketQueue.Instance.Push(i, m);
            };
    }

    public override void OnDisconnected(EndPoint endPoint)
    {
        Console.WriteLine($"OnDisconnected : {endPoint}");
    }

    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        PacketManager.Instance.OnRecvPacket(this, buffer);
    }

    public override void OnSend(int numOfBytes)
    {
        // Console.WriteLine($"Transferred bytes : {numOfBytes}");
    }

}
