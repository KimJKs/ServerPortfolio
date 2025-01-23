using UnityEngine;
using TMPro;
using Protocol;
using Data;

public class ItemLogText : MonoBehaviour
{
    [SerializeField] private TextMeshProUGUI logText;

    public void SetText(ItemInfo itemInfo)
    {
        if (Managers.Data.ItemDict.TryGetValue(itemInfo.TemplateId, out var itemData))
        {
            logText.text = $"<b>{itemData.name}</b> x <b>{itemInfo.Count}</b> 획득하였습니다!";
        }
    }

    public void SetText(CurrencyType currencyType, int count)
    {

        logText.text = $"<b>{count}{currencyType}</b> 획득하였습니다!";
    }

    private void OnEnable()
    {
        Invoke("ReturnToPool", 3f);
    }

    public void ReturnToPool()
    {
        if (this.GetComponent<Poolable>() != null)
            Managers.Pool.Push(this.GetComponent<Poolable>());
    }
}
