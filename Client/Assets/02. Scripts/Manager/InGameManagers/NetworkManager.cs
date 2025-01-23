using ServerCore;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using UnityEngine;
using Google.Protobuf;
using Protocol;

public class NetworkManager
{
	public int AccountId { get; set; }
	public int Token { get; set; }

	ServerSession _serverSession = new ServerSession();

	ServerSession _session = new ServerSession();

	public void Send(IMessage packet)
	{
		_serverSession.Send(packet, ServerType.Main);
	}


	public void Init()
	{

		// 게임 서버 연결 설정 (포트 7777)
		string host = Dns.GetHostName();
		IPHostEntry ipHost = Dns.GetHostEntry(host);
		IPAddress ipAddr = IPAddress.Parse("127.0.0.1");
		IPEndPoint gameServerEndPoint = new IPEndPoint(ipAddr, 7777);
		Connector gameConnector = new Connector();

		gameConnector.Connect(gameServerEndPoint, () =>
	{
		// 서버 세션 반환
		return _serverSession;
	}, 1);


	}

	public void Update()
	{
		List<PacketMessage> gameServerList = PacketQueue.Instance.PopAll();
		foreach (PacketMessage packet in gameServerList)
		{
			Action<PacketSession, IMessage> handler = PacketManager.Instance.GetPacketHandler(packet.Id);
			if (handler != null)
				handler.Invoke(_serverSession, packet.Message);
		}

	}

}
