using Google.Protobuf;
using Protocol;
using TMPro;
using UnityEngine;

public class ChattingManager
{

    public void SendChatMessage(string message)
    {
        if (string.IsNullOrWhiteSpace(message))
            return;

        if (message.StartsWith("/"))
        {
            HandleCommand(message);
        }
        else
        {
            C_CHAT chatPacket = new C_CHAT
            {
                SenderId = Managers.Object.MyPlayer.Id,
                Message = message
            };

            Managers.Network.Send(chatPacket);
        }
    }

    private void HandleCommand(string message)
    {
        string[] splitMessage = message.Split(' ', 3);

        if (splitMessage.Length < 3)
        {
            Managers.UI.ShowChat("잘못된 명령어입니다.", Color.yellow);
            return;
        }

        string command = splitMessage[0].ToLower();
        string targetName = splitMessage[1];
        string content = splitMessage[2];

        switch (command)
        {
            case "/귓속말":
            case "/귓":
            case "/whisper":
            case "/w":
                SendWhisper(targetName, content);
                break;

            default:
                Managers.UI.ShowChat("잘못된 명령어입니다.", Color.yellow);
                break;
        }
    }

    private void SendWhisper(string receiverName, string message)
    {
        if (string.IsNullOrWhiteSpace(receiverName) || string.IsNullOrWhiteSpace(message))
        {
            Managers.UI.ShowChat("잘못된 대상입니다.", Color.red);
            return;
        }

        C_WHISPER whisperPacket = new C_WHISPER
        {
            ReceiverName = receiverName,
            Message = message
        };

        Managers.Network.Send(whisperPacket);
    }

    public void HandleChat(S_CHAT pkt)
    {

        Color fontColor;

        switch (pkt.Type)
        {
            case ChatType.General:
                fontColor = Color.white;
                if (Managers.Object.FindById(pkt.SenderId)) Managers.UI.ShowOverheadDialogue(pkt.SenderId, pkt.Content);
                break;

            case ChatType.Whisper:
                fontColor = Color.blue;
                break;

            case ChatType.System:
                fontColor = Color.yellow;
                break;

            default:
                fontColor = Color.white;
                break;
        }

        // UI 매니저에 전달
        Managers.UI.ShowChat(pkt.Prefix + pkt.Content, fontColor);
    }


}
