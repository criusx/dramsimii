using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;

namespace GenTag_Server
{
    class daemon
    {
        private TcpListener listener;

        public daemon(AsyncCallback callback, int port)
        {
            listener = new TcpListener(System.Net.IPAddress.Any, port);
            listener.Start();
            listener.BeginAcceptTcpClient(callback, listener);
        }

        public TcpClient EndAcceptTcpClient(IAsyncResult asyncResult)
        {
            return listener.EndAcceptTcpClient(asyncResult);
        }

        public void BeginAcceptTcpClient(AsyncCallback callback, object state)
        {
            listener.BeginAcceptTcpClient(callback, state);
        }

        public void Stop()
        {
            listener.Stop();
        }

    }
}
