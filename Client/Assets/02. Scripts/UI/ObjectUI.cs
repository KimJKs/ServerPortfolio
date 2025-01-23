using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;

public class ObjectUI : MonoBehaviour
{
    Transform cam;
    BaseController bc;

    [SerializeField] float Y_value;

    private void Start()
    {
        cam = Camera.main.transform;
        Transform currentParent = transform.parent;
        while (currentParent != null)
        {
            bc = currentParent.GetComponent<BaseController>();
            if (bc != null)
            {
                break; // BaseController를 찾으면 루프 종료
            }
            currentParent = currentParent.parent; // 부모의 부모로 이동
        }
        SetPosition();
    }

    private void Update()
    {
        SetPosition();
    }

    void LateUpdate()
    {

        //   transform.LookAt(transform.position + cam.rotation * Vector3.forward, cam.rotation * Vector3.up);
    }


    public void SetPosition()
    {
        // 화면 좌표로 변환
        Vector3 screenPosition = Camera.main.WorldToScreenPoint(bc.transform.position);
        transform.position = screenPosition + new Vector3(0, Y_value, 0);

    }

}
