using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using Protocol;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Managers : MonoBehaviour
{
    static Managers s_instance;
    public static Managers Instance { get { return s_instance; } }

    #region Contents
    ObjectManager _obj = new ObjectManager();
    NetworkManager _network = new NetworkManager();
    DataManager _data = new DataManager();
    UIManager _ui = new UIManager();
    InventoryManager _inven = new InventoryManager();
    PoolManager _pool = new PoolManager();
    ResourceManager _resource = new ResourceManager();
    InputManager _input = new InputManager();
    ChattingManager _chat = new ChattingManager();
    InteractionManager _interaction = new InteractionManager();
    MarketManager _market = new MarketManager();
    MapManager _map = new MapManager();
    SkillManager _skill = new SkillManager();

    public static ObjectManager Object { get { return Instance._obj; } }
    public static NetworkManager Network { get { return Instance._network; } }
    public static UIManager UI { get { return Instance._ui; } }
    public static DataManager Data { get { return Instance._data; } }
    public static InventoryManager Inven { get { return Instance._inven; } }
    public static PoolManager Pool { get { return Instance._pool; } }
    public static ResourceManager Resource { get { return Instance._resource; } }
    public static InputManager Input { get { return Instance._input; } }
    public static ChattingManager Chat { get { return Instance._chat; } }
    public static InteractionManager Interaction { get { return Instance._interaction; } }
    public static MarketManager Market { get { return Instance._market; } }
    public static MapManager Map { get { return Instance._map; } }
    public static SkillManager Skill { get { return Instance._skill; } }

    #endregion


    void Start()
    {
        Init();

        // StartPingLoop();
    }

    private static ulong _lastSentTimestamp;

    public static void StartPingLoop()
    {
        Task.Run(async () =>
        {
            while (true)
            {
                // 현재 타임스탬프 기록 및 전송
                _lastSentTimestamp = GetCurrentTimestamp();
                C_PONG pingPacket = new C_PONG
                {
                    TimeStamp = _lastSentTimestamp
                };

                Network.Send(pingPacket);
                Console.WriteLine("[Client] Sent C_PING.");

                await Task.Delay(3000); // 3초마다 전송
            }
        });
    }

    private static ulong GetCurrentTimestamp()
    {
        return (ulong)(DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalMilliseconds;
    }

    void Update()
    {
        _network.Update();
        _input.Update();
    }

    static void Init()
    {
        if (s_instance == null)
        {
            GameObject go = GameObject.Find("@Managers");
            if (go == null)
            {
                go = new GameObject { name = "@Managers" };
                go.AddComponent<Managers>();
            }

            DontDestroyOnLoad(go);
            s_instance = go.GetComponent<Managers>();

            s_instance._network.Init();

        }
    }


    public static void InitAfterLogin()
    {
        s_instance._data.Init();
        s_instance._ui.Init();
        s_instance._pool.Init();
        s_instance._input.Init();
        s_instance._map.Init();

        //게임 입장 패킷
        C_ENTER_GAME enterPkt = new C_ENTER_GAME();
        enterPkt.IsDummy = false;
        s_instance._network.Send(enterPkt);
    }

    public static void Clear()
    {

    }
}
