using System.Collections;
using System.Collections.Generic;
using Data;
using Protocol;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class MarketItemSlot : MonoBehaviour
{
    public MarketItemInfo ItemInfo { get; private set; }

    [SerializeField] Image itemIMG;

    [SerializeField] TextMeshProUGUI itemName;

    [SerializeField] TextMeshProUGUI itemCost;

    ItemData itemData;

    public void SetData(MarketItemInfo itemInfo)
    {
        ItemInfo = itemInfo;

        itemData = Managers.Data.ItemDict[ItemInfo.TemplateId];

        itemIMG.sprite = Managers.Resource.Load<Sprite>(itemData.iconPath);
        itemName.text = itemData.name;
        itemCost.text = ItemInfo.Cost.ToString();

        this.GetComponent<Image>().color = Color.white;
    }
}
