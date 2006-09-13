using System;
using System.Collections;
using System.Windows.Forms;
using System.Net.Sockets;
using OwnerDrawnListFWProject;
using ListItemNS;

namespace RFIDProtocolLib
{
    /// <summary>
    /// This is a client connection to the server.
    /// </summary>
    public class ClientConnection
    {
        private TcpClient c;

        private string hostName;

        private int port;

        private string lastDateTime;

        private string containerID;

        private int manifestNum;
        
        /// <summary>
        /// Normal constructor.
        /// </summary>
        public ClientConnection()
        {
            //c = new TcpClient();
        }

        /// <summary>
        /// </summary>
        /// 
        public string LastDateTime
        {
            set { lastDateTime = value; }
        }

        /// <summary>
        /// </summary>
        public string ContainerID
        {
            set { containerID = value; }
        }

        /// <summary>
        /// </summary>
        public int ManifestNum
        {
            set { manifestNum = value; }
        }

        /// <summary>
        /// Sets the hostname
        /// </summary>
        /// <param name="hN"></param>
        public string HostName
        {
            set { hostName = value; }
            get { return hostName; }
        }

        /// <summary>
        /// Sets or gets the host port
        /// </summary>
        public int Port
        {
            set { port = value; }
            get { return port; }
        }

        /// <summary>
        /// Connects to a remote host.
        /// </summary>
        /// <param name="host">The host to connect to.</param>
        /// <param name="port">Its port.</param>
        public void Connect(string host, int portNum)
        {
			hostName = host;
			port = portNum;
            c = new TcpClient(host, portNum);
            //c.SendTimeout = 5;
            //c.ReceiveTimeout = 500;            
        }

        /// <summary>
        /// Close the connection.
        /// </summary>
        public void Close()
        {
            c.Client.Close();
            c.Close();
        }

        public void SendPacket(Packet req)
        {
            TLV connectPacket = new TLV(req.Type, req.ToTLVList().GetBytes());
            connectPacket.WriteToStream(c.GetStream());
        }

        /// <summary>
        /// Wait for ack
        /// </summary>
        private void waitForAck()
        {
            TLV ackPacket = new TLV();
            while (ackPacket.Type != (ushort)Packets.Ack)
                ackPacket.ReadFromStream(c.GetStream());
        }

        /// <summary>
        /// Sent from the server back after a Query packet is sent.
        /// This doesn't do it yet, but will throw an exception if it receives
        /// an ERROR packet.
        /// </summary>
        /// <returns>A QueryResponse describing the RFID.</returns>
        public QueryResponse WaitForQueryResponsePacket()
        {
            TLV responsePacket = new TLV();
            while (responsePacket.Type != (ushort)Packets.QueryResponse)
                responsePacket.ReadFromStream(c.GetStream());

            return new QueryResponse(responsePacket.Value);
        }

        public void Reconcile(string phoneNum)
        {
            try
            {
                // send the reconcile complete command
                Connect(hostName, 1555);
                SendPacket(new ReconcileFinished(lastDateTime, containerID, phoneNum));
                waitForAck();
                Close();
            }
            catch (Exception ex)
            {
                System.Console.WriteLine(ex.Message);

                try
                {
                    Close();
                }
                catch (Exception ex2)
                {
                    System.Console.WriteLine(ex2.Message);
                }
            }
        }

        public void sendAlert()
        {
            // send warning packet
            Connect(hostName, 1555);
            SendPacket(new RaiseAlert());
            Close();
        }

        public void removeItem(string RFIDNum)
        {
            Connect(hostName, 1555);

            // send command to remove this item
            SendPacket(new addRemoveItem(lastDateTime, RFIDNum, containerID, true, manifestNum));

            Close();
        }

        public void addItem(string RFIDNum)
        {
            Connect(hostName, 1555);

            // send command to add this item to the manifest
            SendPacket(new addRemoveItem(lastDateTime, RFIDNum, containerID, false, manifestNum));

            Close();
        }

        public void sendScan(ref ArrayList inventoryTags, double latitude, char NorS, double longitude, char EorW, byte isScan)
        {
            ArrayList invList2 = new ArrayList();
            // connect to the server
            Connect(hostName, 1555);

            // send all the RFIDs to the server
            for (int i = 0; i < inventoryTags.Count; ++i)
                SendPacket(new QueryRequest(new RFID(inventoryTags[i].ToString())));

            lastDateTime = DateTime.Now.ToString();

            SendPacket(new Info(latitude.ToString("N15").Substring(0, 15) + NorS, longitude.ToString("N15").Substring(0, 15) + EorW, lastDateTime, isScan));

            // receive descriptions of items submitted
            manifestNum = -1;

            QueryResponse qr = new QueryResponse("No tags returned",-1," ", 0, " ");

            while (manifestNum == -1)
            {
                qr = WaitForQueryResponsePacket();

                if ((manifestNum = qr.manifestNum) == -1)
                {
                    if (qr.ShortDesc == "Shipping Container 42D")
                        containerID = qr.rfidNum;

                    //custList.Insert(j++, new ListItem(qr.rfidNum, qr.ShortDesc, qr.addedRemoved));
                    invList2.Add(new ListItem(qr.rfidNum, qr.ShortDesc, qr.addedRemoved));
                }
            }
            if (manifestNum == -2)
            {
                throw new Exception(qr.ShortDesc);
            }

            inventoryTags.Clear();
            inventoryTags = invList2;

            Close();
        }
    }
}
