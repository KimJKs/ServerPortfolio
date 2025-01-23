using System;
using System.Collections;
using System.Collections.Generic;
using Protocol;
using UnityEngine;

public class InventoryManager : MonoBehaviour
{
	public Dictionary<int, Item> Items { get; } = new Dictionary<int, Item>();

	public Dictionary<ItemType, Item> EquippedItem { get; } = new Dictionary<ItemType, Item>();

	public int Gold = 0;

	public void Add(Item item)
	{
		if (Items.ContainsKey(item.ItemDbId))
		{
			Items[item.ItemDbId] = item;
		}
		else Items.Add(item.ItemDbId, item);
	}

	public Item Get(int itemDbId)
	{
		Item item = null;
		Items.TryGetValue(itemDbId, out item);
		return item;
	}

	public Item Find(Func<Item, bool> condition)
	{
		foreach (Item item in Items.Values)
		{
			if (condition.Invoke(item))
				return item;
		}

		return null;
	}

	public void Change(Item item)
	{
		if (Items.ContainsKey(item.ItemDbId))
		{
			Items[item.ItemDbId] = item;
		}
	}

	public void Remove(int itemDbId)
	{
		Items.Remove(itemDbId);
	}

	public void Clear()
	{
		Items.Clear();
	}
}