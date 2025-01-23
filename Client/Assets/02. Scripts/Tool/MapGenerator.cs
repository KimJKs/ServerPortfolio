#if UNITY_EDITOR
using System.IO;
using UnityEditor;
using UnityEngine;
using System;

[Serializable]
public class MapData
{
    public int Width; // 맵의 가로 크기
    public int Height; // 맵의 세로 크기
    public string[] Rows; // 각 행의 데이터 (00110 형태로 저장)
}

public class MapDataGenerator : EditorWindow
{
    private int _mapWidth = 20;
    private int _mapHeight = 20;
    private float _cellSize = 1f;
    private LayerMask _obstacleLayer;

    [MenuItem("Tools/Map Data Generator")]
    public static void ShowWindow()
    {
        GetWindow<MapDataGenerator>("Map Data Generator");
    }

    private void OnGUI()
    {
        GUILayout.Label("Map Data Generator", EditorStyles.boldLabel);

        _mapWidth = EditorGUILayout.IntField("Map Width", _mapWidth);
        _mapHeight = EditorGUILayout.IntField("Map Height", _mapHeight);
        _cellSize = EditorGUILayout.FloatField("Cell Size", _cellSize);

        _obstacleLayer = EditorGUILayout.LayerField("Obstacle Layer", _obstacleLayer);

        if (GUILayout.Button("Generate and Save Map Data"))
        {
            GenerateMapData();
        }
    }

    private void GenerateMapData()
    {
        MapData mapData = new MapData
        {
            Width = _mapWidth,
            Height = _mapHeight,
            Rows = new string[_mapHeight]
        };

        for (int y = -_mapHeight / 2; y < _mapHeight / 2; y++)
        {
            string row = "";
            for (int x = -_mapWidth / 2; x < _mapWidth / 2; x++)
            {
                Vector3 worldPosition = new Vector3(x * _cellSize, 0, y * _cellSize);

                bool isObstacle = Physics.CheckSphere(worldPosition, _cellSize / 2, (1 << _obstacleLayer));

                row += isObstacle ? "1" : "0";
            }
            mapData.Rows[y + _mapHeight / 2] = row; // 배열 인덱스 보정
        }

        SaveMapData(mapData);
        Debug.Log("Map data generated and saved!");
    }

    private void SaveMapData(MapData mapData)
    {
        // Resources/Data 경로 설정
        string folderPath = Path.Combine(Application.dataPath, "Resources/Data");

        // 폴더가 없으면 생성
        if (!Directory.Exists(folderPath))
        {
            Directory.CreateDirectory(folderPath);
            Debug.Log($"Created folder at: {folderPath}");
        }

        // 파일 경로 지정
        string filePath = Path.Combine(folderPath, "mapData.json");

        // 맵 데이터 JSON으로 저장
        string json = JsonUtility.ToJson(mapData, true);
        File.WriteAllText(filePath, json);

        Debug.Log($"Map data saved to: {filePath}");
    }
}
#endif