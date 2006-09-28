using System;
using System.Net.Sockets;

namespace RFIDServer
{
	/// <summary>
	/// This is the server daemon that listens for incoming connections.
	/// It spawns off a ServerConnection when a new client connects.
	/// </summary>
	public class Daemon
	{
		private TcpListener listener;

		/// <summary>
		/// Listen on loopback at a specified port.
		/// </summary>
		/// <param name="port">The port to listen on.</param>
		public Daemon(int port)
		{
			listener = new TcpListener(System.Net.IPAddress.Any, port);
		}

		/// <summary>
		/// Start the daemon.
		/// </summary>
		public void Start()
		{
			listener.Start();
		}

		/// <summary>
		/// Stop the daemon.
		/// </summary>
		public void Stop()
		{
			listener.Stop();
		}

		/// <summary>
		/// Accept a connecting client.
		/// NOTE: this blocks!
		/// </summary>
		/// <returns>The server connection that can be used to send and receive.</returns>
		public ServerConnection AcceptClientConnection()
		{
			return new ServerConnection(listener.AcceptTcpClient());
		}

        public void BeginAcceptClientConnection(AsyncCallback callback, object state)
        {
            listener.BeginAcceptTcpClient(callback, state);
        }

        public ServerConnection EndAcceptClientConnection(IAsyncResult asyncResult)
        {
            return new ServerConnection(listener.EndAcceptTcpClient(asyncResult));
        }
	}
}
