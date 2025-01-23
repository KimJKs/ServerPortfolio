using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapManager
{
    private Dictionary<int, GameObject> _mapObjects = new Dictionary<int, GameObject>();

    GameObject _activeMap = null;

    public void Init()
    {
        GameObject fieldMap = GameObject.Find("Map").gameObject;
        AddObject(0, fieldMap);
        GameObject dungeonMap = GameObject.Find("Dungeon").gameObject;
        AddObject(1, dungeonMap);
        dungeonMap.SetActive(false);

        _activeMap = fieldMap;
    }



    public void AddObject(int key, GameObject mapObejct)
    {
        if (!_mapObjects.ContainsKey(key))
        {
            _mapObjects[key] = mapObejct;
        }
    }

    public void ChangeMap(int key)
    {
        _activeMap.SetActive(false);
        _mapObjects[key].SetActive(true);

        _activeMap = _mapObjects[key];
    }
}
