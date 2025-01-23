using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;
using Protocol;
using System.Linq;
using Data;
using Unity.VisualScripting;

public class UIManager
{
    private Dictionary<string, GameObject> _uiElements = new Dictionary<string, GameObject>();

    private Transform _damageTextParent;

    private Transform _itemLogParent;

    private Transform _chatTextParent;

    private Transform _marketItemParent;

    const int MAX_ITEM_SLOT_COUNT = 20;

    private Stack<List<GameObject>> _activePop = new Stack<List<GameObject>>();

    PopupContext _nowOpenPopup = PopupContext.Inventory;

    public void Init()
    {

        //Chat
        Transform chatCanavs = GameObject.Find("ChatCanvas").transform;

        GameObject chatobj = chatCanavs.Find("ChatScrollView").gameObject;
        AddUI("ChatScrollView", chatobj);
        AddUI("ChatContent", chatobj.transform.Find("Viewport").Find("Content").gameObject);
        _chatTextParent = _uiElements["ChatContent"].transform;

        AddUI("ChatInputField", chatobj.transform.Find("ChatInputField").gameObject);
        AddUI("ChatSendBtn", chatobj.transform.Find("Send").gameObject);
        _uiElements["ChatSendBtn"].GetComponent<Button>().onClick.AddListener(() =>
        {
            SendChat();
        });

        //UI 추가 과정
        //HUD
        Transform hudCanvas = GameObject.Find("HUDCanvas").transform;
        AddUI("GoldText", hudCanvas.Find("GoldBG").Find("goldValue").gameObject);
        AddUI("HealthBar_My", hudCanvas.Find("HPBG").gameObject);
        AddUI("ManaBar_My", hudCanvas.Find("MPBG").gameObject);
        AddUI("Mana_My", hudCanvas.Find("MPBG").gameObject);
        AddUI("InteractionNoti", hudCanvas.Find("InteractionNoti").gameObject);

        //스킬 슬롯
        Transform skillParent = hudCanvas.Find("SkillSlots").transform;

        for (int i = 0; i < skillParent.childCount; i++)
        {
            AddUI("SkillSlot_" + (i + 1), skillParent.GetChild(i).gameObject);

        }

        //아이템 로그
        _itemLogParent = hudCanvas.Find("ItemLogBG");

        // DamageText 초기화
        _damageTextParent = hudCanvas.Find("DamageTextParent");


        //팝업
        Transform popCanavs = GameObject.Find("PopCanvas").transform;

        AddUI("PopBG", popCanavs.Find("PopBG").gameObject);

        //팝업 - 인벤
        AddUI("Inventory", popCanavs.Find("InvenPop").gameObject);
        AddUI("Inventory_Items", popCanavs.Find("InvenPop").Find("Items").gameObject);
        AddUI("Inventory_Equip", popCanavs.Find("InvenPop").Find("Equip").gameObject);
        AddUI("Inventory_Equip_Weapon", _uiElements["Inventory_Equip"].transform.Find("WeaponSlot").gameObject);
        AddUI("Inventory_Equip_Armor", _uiElements["Inventory_Equip"].transform.Find("ArmorSlot").gameObject);
        AddUI("Inventory_Equip_Stat_HP", _uiElements["Inventory_Equip"].transform.Find("Stat_HP").gameObject);
        AddUI("Inventory_Equip_Stat_ATK", _uiElements["Inventory_Equip"].transform.Find("Stat_ATK").gameObject);
        AddUI("Inventory_Equip_Stat_DEF", _uiElements["Inventory_Equip"].transform.Find("Stat_DEF").gameObject);

        AddUI("ItemUseButtonPop", popCanavs.Find("InvenPop").Find("ItemUseButtons").gameObject);

        for (int i = 0; i < MAX_ITEM_SLOT_COUNT; i++)
        {
            AddUI($"Inventory_Slot_{i}", _uiElements["Inventory_Items"].transform.GetChild(i).gameObject);
        }

        //팝업 - 마켓
        AddUI("Market", popCanavs.Find("MarketPop").gameObject);
        AddUI("MarketItemButtonPop", popCanavs.Find("InvenPop").Find("MarketItemButtons").gameObject);
        AddUI("MarketSearchInputField", popCanavs.Find("MarketPop").Find("SearchInputField").gameObject);

        _marketItemParent = _uiElements["Market"].transform.Find("ItemScrollview").GetComponent<ScrollRect>().content;

        Button searchBtnInMarket = _uiElements["Market"].transform.Find("SearchBtn").GetComponent<Button>();
        searchBtnInMarket.onClick.RemoveAllListeners();
        searchBtnInMarket.onClick.AddListener(() => SearchItemInMarket());

        Button purchaseBtnInMarket = _uiElements["Market"].transform.Find("PurchaseBtn").GetComponent<Button>();
        purchaseBtnInMarket.onClick.RemoveAllListeners();
        purchaseBtnInMarket.onClick.AddListener(() =>
        Managers.Market.PurchaseMarketItem(_marketItemParent.GetComponent<ToggleGroup>().ActiveToggles().ToList()[0].GetComponent<MarketItemSlot>().ItemInfo.ItemDbId)
        );

        Button sellBtnInMarket = _uiElements["Market"].transform.Find("SellBtn").GetComponent<Button>();
        sellBtnInMarket.onClick.RemoveAllListeners();
        sellBtnInMarket.onClick.AddListener(() => ActivePopInventory(PopupContext.Market));


    }

    public void UpdateHealthBar(int objectId, int currentHp, int maxHp, bool isMy = false)
    {
        string keyValue = isMy ? "HealthBar_My" : $"HealthBar_{objectId}";

        if (!_uiElements.TryGetValue(keyValue, out GameObject healthBar))
            return;

        // HealthBar UI 갱신 로직
        Image hpBG = healthBar.GetComponent<Image>();
        Image hpValue = healthBar.transform.GetChild(0).GetComponent<Image>();

        float hpRatio = (float)currentHp / (float)maxHp;
        hpValue.rectTransform.sizeDelta = new Vector2(hpBG.rectTransform.sizeDelta.x * hpRatio, hpValue.rectTransform.sizeDelta.y);

    }

    public void UpdateManaBar(int objectId, int currentMp, int maxMp, bool isMy = false)
    {
        string keyValue = isMy ? "ManaBar_My" : $"ManaBar_{objectId}";

        if (!_uiElements.TryGetValue(keyValue, out GameObject manaBar))
            return;

        // HealthBar UI 갱신 로직
        Image mpBG = manaBar.GetComponent<Image>();
        Image mpValue = manaBar.transform.GetChild(0).GetComponent<Image>();

        float mpRatio = (float)currentMp / (float)maxMp;
        mpValue.rectTransform.sizeDelta = new Vector2(mpBG.rectTransform.sizeDelta.x * mpRatio, mpValue.rectTransform.sizeDelta.y);

    }
    public void ShowGold()
    {
        // 골드 UI 갱신 로직
        if (_uiElements.TryGetValue("GoldText", out GameObject goldText))
        {
            var text = goldText.GetComponent<TextMeshProUGUI>();
            if (text != null)
                text.text = Managers.Inven.Gold.ToString();
        }
    }

    public void AddUI(string key, GameObject uiElement)
    {
        _uiElements[key] = uiElement;
    }

    public void RemoveUI(string key)
    {
        if (_uiElements.ContainsKey(key))
        {
            _uiElements.Remove(key);
        }
    }

    public void ShowDamageText(float damage, Vector3 worldPosition)
    {
        GameObject go = Managers.Resource.Instantiate("UI/DamageText", _damageTextParent);

        DamageText text = go.GetComponent<DamageText>();
        text.SetText(damage);
        text.Play(worldPosition);
    }

    public void ShowItemLog(ItemInfo itemInfo)
    {
        GameObject go = Managers.Resource.Instantiate("UI/ItemLogText", _itemLogParent);

        ItemLogText text = go.GetComponent<ItemLogText>();
        text.SetText(itemInfo);

        // 레이아웃 강제 갱신
        LayoutRebuilder.ForceRebuildLayoutImmediate(_itemLogParent.GetComponent<RectTransform>());
    }

    public void ShowCurrencyLog(CurrencyType type, int count)
    {
        GameObject go = Managers.Resource.Instantiate("UI/ItemLogText", _itemLogParent);

        ItemLogText text = go.GetComponent<ItemLogText>();
        text.SetText(type, count);

        // 레이아웃 강제 갱신
        LayoutRebuilder.ForceRebuildLayoutImmediate(_itemLogParent.GetComponent<RectTransform>());
    }

    public void SetActiveInteractionNoti(bool isActive)
    {
        _uiElements["InteractionNoti"].gameObject.SetActive(isActive);
    }

    public void InActivePop()
    {
        if (_activePop.Count <= 0)
            return;

        List<GameObject> popObjs = _activePop.Pop();

        foreach (GameObject obj in popObjs)
        {
            obj.SetActive(false);
        }
    }

    #region 스킬

    private Dictionary<int, Coroutine> _skillCoroutines = new Dictionary<int, Coroutine>();

    public void StartSkillCooltime(int skillCoolTime, int skillId)
    {


        Coroutine skillCoolTimeCorountine = CoroutineManager.Instance.StartManagedCoroutine(CO_StartSkillCoolTime(skillCoolTime, skillId));

        _skillCoroutines.Add(skillId, skillCoolTimeCorountine);
    }

    IEnumerator CO_StartSkillCoolTime(int skillCoolTime, int skillId)
    {
        float skillCoolTimeSeconds = skillCoolTime / 1000f;

        float elapsedTime = 0f;

        Image coolTimeImage = _uiElements["SkillSlot_" + skillId].transform.Find("Cooltime").GetComponent<Image>();


        while (elapsedTime < skillCoolTimeSeconds)
        {
            elapsedTime += Time.deltaTime;
            coolTimeImage.fillAmount = 1 - (elapsedTime / skillCoolTimeSeconds); // 남은 쿨타임 비율 계산
            yield return null;
        }

        // 쿨타임 종료 처리
        coolTimeImage.fillAmount = 0; // 오버레이 숨김

        Managers.Skill.CanUseSkill[skillId] = true;

        _skillCoroutines.Remove(skillId);
    }

    #endregion

    #region 마켓

    public void ActivePopMarket()
    {
        _nowOpenPopup = PopupContext.Market;
        GameObject market = _uiElements["Market"];
        GameObject popBG = _uiElements["PopBG"];

        if (market.activeSelf)
            return;

        market.SetActive(true);
        popBG.SetActive(true);

        _activePop.Push(new List<GameObject> { popBG, market });

        C_REQUEST_MARKET_ITEM_LIST reqPkt = new C_REQUEST_MARKET_ITEM_LIST();
        Managers.Network.Send(reqPkt);
    }

    public void RefreshMarketUI(IEnumerable<MarketItemInfo> items)
    {
        List<Toggle> activeToggle = _marketItemParent.GetComponent<ToggleGroup>().ActiveToggles().ToList();
        foreach (Toggle tg in activeToggle)
        {
            tg.isOn = false;
        }

        // 기존 UI 슬롯 초기화
        for (int i = _marketItemParent.childCount - 1; i >= 0; i--)
        {
            Managers.Resource.Destroy(_marketItemParent.GetChild(i).gameObject);
        }

        // 새로운 아이템 목록 표시
        foreach (MarketItemInfo itemInfo in items)
        {
            MarketItemSlot itemSlot = Managers.Resource.Instantiate("UI/MarketItemSlot", _marketItemParent).GetComponent<MarketItemSlot>();
            itemSlot.SetData(itemInfo);

            itemSlot.GetComponent<Toggle>().group = _marketItemParent.GetComponent<ToggleGroup>();
        }
    }

    public void ShowItemPopupInMarket(Item item, Vector3 position)
    {
        GameObject popup = _uiElements["MarketItemButtonPop"];
        popup.SetActive(true);
        popup.transform.position = position;

        TMP_InputField costInputField = popup.transform.Find("ItemCostInputField").GetComponent<TMP_InputField>();
        Button registerButton = popup.transform.Find("Register").GetComponent<Button>();

        registerButton.onClick.RemoveAllListeners();
        registerButton.onClick.AddListener(() =>
        {
            if (int.TryParse(costInputField.text, out int cost))
            {
                Managers.Market.RegisterItemInMarket(item, cost);
                popup.SetActive(false);
            }
        });
    }

    public void SearchItemInMarket()
    {
        string inputText = _uiElements["MarketSearchInputField"].GetComponent<TMP_InputField>().text;

        Managers.Market.FindItemListByName(inputText);
    }

    #endregion

    #region  아이템 인벤토리
    public void ActivePopInventory(PopupContext context = PopupContext.Inventory)
    {
        if (_nowOpenPopup != PopupContext.Inventory) // 이미 열려 있는 경우
        {
            _nowOpenPopup = context; // 경매장에서 호출 시 덮어쓰지 않도록
        }

        GameObject inven = _uiElements["Inventory"];
        GameObject popBG = _uiElements["PopBG"];
        SetStatUI();

        if (inven.activeSelf)
            return;

        inven.SetActive(true);
        popBG.SetActive(true);

        _activePop.Push(new List<GameObject> { inven, popBG });
    }

    public void RefreshInventory()
    {
        List<Item> items = Managers.Inven.Items.Values.Where(i => i.isOnMarket == false).OrderBy(i => i.Slot).ToList();



        for (int i = 0; i < items.Count; i++)
        {
            Data.ItemData itemData = null;
            Managers.Data.ItemDict.TryGetValue(items[i].TemplateId, out itemData);

            if (itemData == null) continue;

            GameObject slotObj = _uiElements[$"Inventory_Slot_{i}"];
            GameObject img = slotObj.transform.Find("IMG").gameObject;

            img.SetActive(true);
            img.GetComponent<Image>().sprite = Managers.Resource.Load<Sprite>(itemData.iconPath);

            // 클릭 이벤트 추가
            int index = i; // 로컬 변수로 캡처
            slotObj.GetComponent<Button>().onClick.RemoveAllListeners();
            slotObj.GetComponent<Button>().onClick.AddListener(() =>
            {
                if (_nowOpenPopup == PopupContext.Inventory) ShowItemPopup(items[index], Input.mousePosition);
                else if (_nowOpenPopup == PopupContext.Market) ShowItemPopupInMarket(items[index], Input.mousePosition);
            });

            GameObject countObj = slotObj.transform.Find("Count").gameObject;
            GameObject equipObj = slotObj.transform.Find("isEquiped").gameObject;

            equipObj.SetActive(false);
            countObj.SetActive(false);
            if (itemData.itemType == ItemType.Consumable)
            {
                countObj.SetActive(true);
                countObj.GetComponent<TextMeshProUGUI>().text = items[i].Count.ToString();
            }
            else if (items[i].Equipped)
            {


                equipObj.SetActive(true);
            }

        }

        // 빈 슬롯 초기화
        for (int i = items.Count; i < MAX_ITEM_SLOT_COUNT; i++)
        {
            GameObject slotObj = _uiElements[$"Inventory_Slot_{i}"];
            GameObject img = slotObj.transform.Find("IMG").gameObject;
            GameObject countObj = slotObj.transform.Find("Count").gameObject;
            GameObject equipObj = slotObj.transform.Find("isEquiped").gameObject;

            // 빈 슬롯 이미지 및 텍스트 숨기기
            img.SetActive(false);
            countObj.SetActive(false);
            equipObj.SetActive(false);

            // 클릭 이벤트 제거
            slotObj.GetComponent<Button>().onClick.RemoveAllListeners();
        }
    }

    public void SetStatUI()
    {
        WeaponData weaponData = null;
        int weaponPlus = 0;
        if (Managers.Inven.EquippedItem.ContainsKey(ItemType.Weapon))
        {
            weaponData = (WeaponData)Managers.Data.ItemDict[Managers.Inven.EquippedItem[ItemType.Weapon].TemplateId];
            weaponPlus = weaponData.effects[0].value;
        }

        ArmorData armorData = null;
        int armorPlus = 0;
        if (Managers.Inven.EquippedItem.ContainsKey(ItemType.Armor))
        {
            armorData = (ArmorData)Managers.Data.ItemDict[Managers.Inven.EquippedItem[ItemType.Armor].TemplateId];
            armorPlus = armorData.effects[0].value;
        }


        if (weaponData != null)
        {
            _uiElements["Inventory_Equip_Weapon"].transform.Find("IMG").gameObject.SetActive(true);
            _uiElements["Inventory_Equip_Weapon"].transform.Find("IMG").GetComponent<Image>().sprite = Managers.Resource.Load<Sprite>(weaponData.iconPath);
        }
        else _uiElements["Inventory_Equip_Weapon"].transform.Find("IMG").gameObject.SetActive(false);

        _uiElements["Inventory_Equip_Stat_ATK"].transform.GetComponent<TextMeshProUGUI>().text = "ATK : " + (Managers.Object.MyPlayer.Stat.Attack + weaponPlus).ToString();

        if (armorData != null)
        {
            _uiElements["Inventory_Equip_Armor"].transform.Find("IMG").gameObject.SetActive(true);
            _uiElements["Inventory_Equip_Armor"].transform.Find("IMG").GetComponent<Image>().sprite = Managers.Resource.Load<Sprite>(armorData.iconPath);
        }
        else _uiElements["Inventory_Equip_Armor"].transform.Find("IMG").gameObject.SetActive(false);

        _uiElements["Inventory_Equip_Stat_DEF"].transform.GetComponent<TextMeshProUGUI>().text = "DEF : " + (Managers.Object.MyPlayer.Stat.Defense + armorPlus).ToString();


        _uiElements["Inventory_Equip_Stat_HP"].transform.GetComponent<TextMeshProUGUI>().text = "HP : " + Managers.Object.MyPlayer.Stat.Hp + " / " + Managers.Object.MyPlayer.Stat.MaxHp;

    }


    public void ShowItemPopup(Item item, Vector3 position)
    {
        // 팝업 UI를 생성하거나 활성화
        GameObject popup = _uiElements["ItemUseButtonPop"];

        ItemData itemData = Managers.Data.ItemDict[item.TemplateId];

        popup.SetActive(true);

        // 팝업 위치 설정 (마우스 위치 기준으로 표시)
        popup.transform.position = position;

        // 팝업 내용 설정
        popup.transform.Find("ItemName").GetComponent<TextMeshProUGUI>().text = itemData.name.ToString();

        Button useBtn = popup.transform.Find("Use").GetComponent<Button>();
        useBtn.onClick.RemoveAllListeners();
        useBtn.onClick.AddListener(() => UseItem(item));

        string useString = item.ItemType == ItemType.Consumable ? "사용하기" : item.Equipped ? "해제하기" : "착용하기";
        useBtn.transform.GetComponentInChildren<TextMeshProUGUI>().text = useString;

        popup.transform.Find("Drop").GetComponent<Button>().onClick.RemoveAllListeners();
        popup.transform.Find("Drop").GetComponent<Button>().onClick.AddListener(() => DropItem(item));
    }

    public void HideItemPopup()
    {
        if (_uiElements.TryGetValue("ItemUseButtonPop", out GameObject popup))
        {
            popup.SetActive(false);
        }
    }

    private void UseItem(Item item)
    {
        C_USE_ITEM useitemPacket = new C_USE_ITEM
        {
            ItemDbId = item.ItemDbId
        };
        Managers.Network.Send(useitemPacket);

        HideItemPopup();
    }

    private void DropItem(Item item)
    {
        C_DROP_ITEM useitemPacket = new C_DROP_ITEM
        {
            ItemDbId = item.ItemDbId
        };
        Managers.Network.Send(useitemPacket);

        HideItemPopup();
    }
    #endregion

    #region  채팅

    public void ShowChat(string msg, Color textColor)
    {
        TextMeshProUGUI text = Managers.Resource.Instantiate("UI/ChatText", _chatTextParent).GetComponent<TextMeshProUGUI>();

        text.text = msg;
        text.color = textColor;
        LayoutRebuilder.ForceRebuildLayoutImmediate(_chatTextParent.GetComponent<RectTransform>());
    }

    public void SendChat()
    {
        TMP_InputField inputField = _uiElements["ChatInputField"].GetComponent<TMP_InputField>();

        Managers.Chat.SendChatMessage(inputField.text);

        inputField.text = "";
    }

    public void ShowOverheadDialogue(int senderID, string content)
    {
        _uiElements[$"Overhead_Dialogue_{senderID}"].SetActive(true);
        _uiElements[$"Overhead_Dialogue_{senderID}"].GetComponent<DialoguePop>().ShowMessage(content);
    }

    #endregion
}

public enum PopupContext
{
    Inventory,
    Market
}