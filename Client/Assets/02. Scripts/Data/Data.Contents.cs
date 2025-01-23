using Protocol;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Data
{
	#region Item
	[Serializable]
	public class ItemData
	{
		public int id;
		public string name;
		public ItemType itemType;
		public string iconPath;
		public List<EffectData> effects; // 추가된 필드
	}

	[Serializable]
	public class WeaponData : ItemData
	{
		public WeaponType weaponType;
		public int damage;
	}

	[Serializable]
	public class ArmorData : ItemData
	{
		public ArmorType armorType;
		public int defence;
	}

	[Serializable]
	public class ConsumableData : ItemData
	{
		public ConsumableType consumableType;
		public int maxCount;
	}

	[Serializable]
	public class EffectData : ItemData
	{
		public string effectType;
		public int value;
	}


	[Serializable]
	public class ItemLoader : ILoader<int, ItemData>
	{
		public List<WeaponData> weapons = new List<WeaponData>();
		public List<ArmorData> armors = new List<ArmorData>();
		public List<ConsumableData> consumables = new List<ConsumableData>();

		public Dictionary<int, ItemData> MakeDict()
		{
			Dictionary<int, ItemData> dict = new Dictionary<int, ItemData>();
			foreach (ItemData item in weapons)
			{
				item.itemType = ItemType.Weapon;
				dict.Add(item.id, item);
			}
			foreach (ItemData item in armors)
			{
				item.itemType = ItemType.Armor;
				dict.Add(item.id, item);
			}
			foreach (ItemData item in consumables)
			{
				item.itemType = ItemType.Consumable;
				dict.Add(item.id, item);
			}
			return dict;
		}
	}
	#endregion

	#region Monster

	[Serializable]
	public class MonsterData
	{
		public int id;
		public string name;
		public StatInfo stat;
		public string prefabPath;
	}

	[Serializable]
	public class MonsterLoader : ILoader<int, MonsterData>
	{
		public List<MonsterData> monsters = new List<MonsterData>();

		public Dictionary<int, MonsterData> MakeDict()
		{
			Dictionary<int, MonsterData> dict = new Dictionary<int, MonsterData>();
			foreach (MonsterData monster in monsters)
			{
				dict.Add(monster.id, monster);
			}
			return dict;
		}
	}

	#endregion

	[Serializable]
	public class SkillData
	{
		public int id;
		public string type;
		public string name;
		public string description;
		public string prefab;
		public int cooldown;
		public int cost;
		public int damage;
	}

	[Serializable]
	public class AreaSkill : SkillData
	{
		public float range;
		public float duration;
	}

	[Serializable]
	public class DirectionalSkill : SkillData
	{
		public float angle;
		public float range;
	}

	[Serializable]
	public class SkillLoader : ILoader<int, SkillData>
	{
		public List<AreaSkill> areaSkills = new List<AreaSkill>();
		public List<DirectionalSkill> directionalSkills = new List<DirectionalSkill>();

		public Dictionary<int, SkillData> MakeDict()
		{
			Dictionary<int, SkillData> dict = new Dictionary<int, SkillData>();

			foreach (var skill in areaSkills)
			{
				dict.Add(skill.id, skill);
			}

			foreach (var skill in directionalSkills)
			{
				dict.Add(skill.id, skill);
			}

			return dict;
		}

	}
}
