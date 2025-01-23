using System;
using System.Net;
using System.Net.Sockets;
using UnityEngine;

namespace ServerCore
{
    public class Connector
    {
        Func<Session> _sessionFactory;
        Action _onConnectedCallback; // 연결 완료 후 실행할 콜백

        // 콜백 함수를 추가 인자로 받도록 수정
        public void Connect(IPEndPoint endPoint, Func<Session> sessionFactory, int count = 1, Action onConnectedCallback = null)
        {

            _onConnectedCallback = onConnectedCallback; // 콜백 저장

            for (int i = 0; i < count; i++)
            {
                Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                _sessionFactory = sessionFactory;

                SocketAsyncEventArgs args = new SocketAsyncEventArgs();
                args.Completed += OnConnectCompleted;
                args.RemoteEndPoint = endPoint;
                args.UserToken = socket;

                RegisterConnect(args);
            }
        }

        void RegisterConnect(SocketAsyncEventArgs args)
        {
            Socket socket = args.UserToken as Socket;

            if (socket == null)
            {
                return;
            }

            bool pending = socket.ConnectAsync(args);
            if (pending == false)
            {
                OnConnectCompleted(null, args);
            }
        }

        void OnConnectCompleted(object sender, SocketAsyncEventArgs args)
        {
            if (args.SocketError == SocketError.Success)
            {
                Session session = _sessionFactory.Invoke();
                session.Start(args.ConnectSocket);
                session.OnConnected(args.RemoteEndPoint);
                // 연결이 완료되면 저장된 콜백 함수 호출
                _onConnectedCallback?.Invoke();
            }
            else
            {
                Console.WriteLine($"OnConnectCompleted Fail : {args.SocketError}");
            }
        }
    }
}
