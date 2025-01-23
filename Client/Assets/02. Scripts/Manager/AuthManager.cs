using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
using System.Text;
using TMPro;
using UnityEngine.SceneManagement;
using Protocol;

public class AuthManager : MonoBehaviour
{
    [SerializeField] TMP_InputField register_idInput;
    [SerializeField] TMP_InputField register_passwordInput;
    [SerializeField] TMP_InputField login_idInput;
    [SerializeField] TMP_InputField login_passwordInput;


    public void Register()
    {
        string username = register_idInput.text;
        string password = register_passwordInput.text;

        C_SIGN_UP packet = new C_SIGN_UP
        {
            Id = username,
            Password = password
        };

        Managers.Network.Send(packet);
    }

    // 로그인 함수
    public void Login()
    {
        string username = login_idInput.text;
        string password = login_passwordInput.text;

        C_LOGIN packet = new C_LOGIN
        {
            Id = username,
            Password = password
        };

        Managers.Network.Send(packet);
    }

}
