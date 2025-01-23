using Protocol;
using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class ObjectManager
{
	public MyPlayerController MyPlayer { get; set; }
	Dictionary<int, GameObject> _objects = new Dictionary<int, GameObject>();

	public static ObjectType GetObjectTypeById(int id)
	{
		int type = (id >> 24) & 0x7F;
		return (ObjectType)type;
	}

	public void Add(ObjectInfo info, bool myPlayer = false)
	{
		if (MyPlayer != null && MyPlayer.Id == info.ObjectId)
			return;
		if (_objects.ContainsKey(info.ObjectId))
			return;

		ObjectType objectType = GetObjectTypeById(info.ObjectId);

		if (objectType == ObjectType.Player)
		{
			if (myPlayer)
			{
				GameObject go = Managers.Resource.Instantiate("Player");
				go.transform.position = new Vector3(info.PosInfo.PosX, 0, info.PosInfo.PosZ);

				go.AddComponent<MyPlayerController>();
				_objects.Add(info.ObjectId, go);

				MyPlayer = go.GetComponent<MyPlayerController>();
				MyPlayer.Id = info.ObjectId;

				MyPlayer.SetNameText(info.Name);
				MyPlayer.PosInfo = info.PosInfo;
				MyPlayer.Stat.MergeFrom(info.StatInfo);
				Managers.UI.RefreshInventory();
			}
			else
			{
				GameObject go = Managers.Resource.Instantiate("Player");
				go.transform.position = new Vector3(info.PosInfo.PosX, 0, info.PosInfo.PosZ);
				go.AddComponent<PlayerController>();
				_objects.Add(info.ObjectId, go);

				PlayerController pc = go.GetComponent<PlayerController>();

				pc.SetNameText(info.Name);
				pc.Id = info.ObjectId;
				pc.PosInfo = info.PosInfo;
				pc.Stat.MergeFrom(info.StatInfo);
			}
		}
		else if (objectType == ObjectType.Monster)
		{
			GameObject go = Managers.Resource.Instantiate("Monster");
			go.transform.position = new Vector3(info.PosInfo.PosX, 0, info.PosInfo.PosZ);

			MonsterController mc;

			if (!go.TryGetComponent(out mc))
			{
				mc = go.AddComponent<MonsterController>();
			}

			_objects.Add(info.ObjectId, go);

			mc.Id = info.ObjectId;
			mc.PosInfo = info.PosInfo;
			mc.Stat = info.StatInfo;

			mc.Init();
		}

	}

	public void Remove(int id)
	{
		if (MyPlayer != null && MyPlayer.Id == id)
			return;
		if (_objects.ContainsKey(id) == false)
			return;

		GameObject go = FindById(id);

		_objects.Remove(id);

		if (go == null)
			return;

		Managers.Resource.Destroy(go);

	}

	public GameObject FindById(int id)
	{
		GameObject go = null;
		_objects.TryGetValue(id, out go);
		return go;
	}

	public bool Has(int id)
	{
		if (_objects.ContainsKey(id))
			return true;

		return false;
	}

	public GameObject FindCreature(Vector3Int cellPos)
	{
		foreach (GameObject obj in _objects.Values)
		{
			CreatureController cc = obj.GetComponent<CreatureController>();
			if (cc == null)
				continue;

		}

		return null;
	}

	public GameObject Find(Func<GameObject, bool> condition)
	{
		foreach (GameObject obj in _objects.Values)
		{
			if (condition.Invoke(obj))
				return obj;
		}

		return null;
	}

	public void Clear()
	{
		_objects.Clear();
		MyPlayer = null;
	}
}
