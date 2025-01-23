using UnityEngine;
using UnityEngine.EventSystems;

public class CloseOnClickOutside : MonoBehaviour
{
    private GameObject uiPanel; // 닫힐 UI 패널

    private void Awake()
    {
        uiPanel = gameObject; // 연결하지 않으면 자신을 대상으로 설정
    }

    private void Update()
    {
        // 마우스 클릭을 감지
        if (Input.GetMouseButtonDown(0)) // 왼쪽 클릭
        {
            // 클릭한 위치가 UI 내부가 아니면 닫기
            if (!RectTransformUtility.RectangleContainsScreenPoint(uiPanel.GetComponent<RectTransform>(), Input.mousePosition))
            {
                CloseUI();
            }
        }
    }

    private void CloseUI()
    {
        uiPanel.SetActive(false);
    }
}
