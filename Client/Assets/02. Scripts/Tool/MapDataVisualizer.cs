using UnityEngine;

public class MapDataVisualizer : MonoBehaviour
{
    public int MapWidth = 10;
    public int MapHeight = 10;
    public float CellSize = 1f;
    public LayerMask ObstacleLayer;

    public bool isVisual = true;

    private void OnDrawGizmos()
    {
        if (!isVisual) return;

        for (int x = -MapWidth / 2; x < MapWidth / 2; x++)
        {
            for (int y = -MapHeight / 2; y < MapHeight / 2; y++)
            {
                Vector3 worldPosition = new Vector3(x * CellSize, 0, y * CellSize);

                // CheckSphere 결과에 따라 색상 변경
                bool isObstacle = Physics.CheckSphere(worldPosition, CellSize / 2, ObstacleLayer.value);
                Gizmos.color = isObstacle ? Color.red : Color.green;

                Gizmos.DrawWireSphere(worldPosition, CellSize / 2);
            }
        }
    }
}
