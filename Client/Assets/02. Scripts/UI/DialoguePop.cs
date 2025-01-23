using System.Collections;
using TMPro;
using UnityEngine;

public class DialoguePop : MonoBehaviour
{
    [SerializeField] private TextMeshProUGUI text;
    private Coroutine hideCoroutine;

    public void ShowMessage(string message)
    {
        // 메시지 설정
        text.text = message;

        // 오브젝트 활성화
        gameObject.SetActive(true);

        // 기존에 실행 중인 코루틴이 있다면 중지
        if (hideCoroutine != null)
        {
            StopCoroutine(hideCoroutine);
        }

        // 3초 후 비활성화를 위한 코루틴 시작
        hideCoroutine = StartCoroutine(HideAfterDelay(3f));
    }

    private IEnumerator HideAfterDelay(float delay)
    {
        yield return new WaitForSeconds(delay);

        // 오브젝트 비활성화
        gameObject.SetActive(false);
    }
}
