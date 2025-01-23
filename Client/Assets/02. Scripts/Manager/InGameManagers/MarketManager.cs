using System.Collections.Generic;
using System.Linq;
using Data;
using Protocol;
using UnityEngine;

public class MarketManager
{
    public Dictionary<int, MarketItemInfo> MarketItems { get; private set; } = new Dictionary<int, MarketItemInfo>();

    public void HandleMarketItemList(S_MARKET_ITEM_LIST pkt)
    {
        MarketItems.Clear();

        foreach (MarketItemInfo info in pkt.Items)
        {
            MarketItems.Add(info.ItemDbId, info);
        }

        // UI 업데이트 요청
        Managers.UI.RefreshMarketUI(MarketItems.Values);
    }

    public void FindItemListByName(string name)
    {
        if (string.IsNullOrEmpty(name))
            Managers.UI.RefreshMarketUI(MarketItems.Values);

        // 이름으로 필터링
        IEnumerable<MarketItemInfo> filteredItems = MarketItems.Values
            .Where(item => Managers.Data.ItemDict[item.TemplateId].name.Contains(name, System.StringComparison.OrdinalIgnoreCase));

        // UI 갱신
        Managers.UI.RefreshMarketUI(filteredItems);
    }

    public void RegisterItemInMarket(Item item, int cost)
    {
        if (cost <= 0) return;

        C_REGISTER_MARKET_ITEM pkt = new C_REGISTER_MARKET_ITEM
        {
            ItemdDbId = item.ItemDbId,
            Cost = cost
        };

        Managers.Network.Send(pkt);
    }

    public void PurchaseMarketItem(int itemDbId)
    {
        if (!MarketItems.ContainsKey(itemDbId)) return;

        C_PURCHASE_MARKET_ITEM pkt = new C_PURCHASE_MARKET_ITEM
        {
            ItemdDbId = itemDbId
        };

        Managers.Network.Send(pkt);
    }
}
