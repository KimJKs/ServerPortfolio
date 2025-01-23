using System.Collections;
using System.Collections.Generic;
using Protocol;
using UnityEngine;

public class RoomEntrance : InteractalbeObject
{
    [SerializeField] int mapId;
    public override void Interact()
    {

        C_ENTER_ROOM packet = new C_ENTER_ROOM
        {
            MapId = mapId,
        };
        Managers.Network.Send(packet);  // 서버로 이동 요청 패킷 전송
    }
}
