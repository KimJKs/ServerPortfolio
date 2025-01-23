using UnityEngine;
using TMPro;

public class DamageText : MonoBehaviour
{
    [SerializeField] private TextMeshProUGUI damageText;

    public Vector3 MovingVector;

    Vector3 _playerPosition = new Vector3();

    public void SetText(float damage)
    {
        damageText.text = damage.ToString();
    }

    public void Play(Vector3 worldPosition)
    {
        // 화면 좌표로 변환
        Vector3 screenPosition = Camera.main.WorldToScreenPoint(worldPosition);
        _playerPosition = screenPosition + new Vector3(0, 30, 0);
        transform.position = _playerPosition;

        // 애니메이션 실행
        Animator animator = GetComponent<Animator>();
        animator.SetTrigger("Show");
    }

    private void Update()
    {
        transform.position = _playerPosition + MovingVector;
    }

    public void ReturnToPool()
    {
        if (this.GetComponent<Poolable>() != null)
            Managers.Pool.Push(this.GetComponent<Poolable>());
    }
}
